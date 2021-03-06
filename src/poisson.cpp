
#include "poisson.h"
#include "filtering.h"

using namespace std;


///////////////////////////////////////////////////////////////
// Poisson Image blending
//////////////////////////////////////////////////////////////
FloatImage Poisson_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes,
                      bool mixGrad, bool isLog) {

    FloatImage result(imDes.width(), imDes.height(), imDes.channels()), poiDes(imDes);
    
    if (isLog) {
        poiDes = log10FloatImage(imDes);
    }

    // get matrix A, same for all channels
    printf("Matrix A \n");
    SparseMatrix<float> A = getA_2D(maskDes);

    // for each channels, get vector b
    printf("Vector b \n");
    VectorXf br = getB_2D(imSrc, poiDes, maskSrc, maskDes, 0, mixGrad);
    VectorXf bg = getB_2D(imSrc, poiDes, maskSrc, maskDes, 1, mixGrad);
    VectorXf bb = getB_2D(imSrc, poiDes, maskSrc, maskDes, 2, mixGrad);

    // solve for x
    printf("Solve xr \n");
    FloatImage xr = solve_2D(imDes, A, br);
    printf("Solve xg \n");
    FloatImage xg = solve_2D(imDes, A, bg);
    printf("Solve xb \n");
    FloatImage xb = solve_2D(imDes, A, bb);

    // combine channels
    printf("Combine channels \n");
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            result(i, j, 0) = xr(i, j, 0);
            result(i, j, 1) = xg(i, j, 0);
            result(i, j, 2) = xb(i, j, 0);
        }
    }
    
    if (isLog) {
        return exp10FloatImage(result);
    }else
        return result;

}


SparseMatrix<float> getA_2D(const FloatImage &maskDes) {
    int N = maskDes.width() * maskDes.height(); // CHANGE ME
    SparseMatrix<float> A(N, N);

    // https://eigen.tuxfamily.org/dox/group__SparseQuickRefPage.html
    vector<Triplet<float>> tripletList;
    for (int i = 0; i < maskDes.width(); i++) {
        for (int j = 0; j < maskDes.height(); j++) {
            if (maskDes(i, j, 0) < 0.5f) { // if is not white
                int d = j * maskDes.width() + i;
                tripletList.push_back(Triplet<float>(d, d, 4.0f));

                // for each neighbor (i+1, j), (i-1, j), (i, j-1), (i, j+1), check if in mask
                int n;
                n = j * maskDes.width() + (i+1);
                if (maskDes.smartAccessor(i+1, j, 0) < 0.5f && i+1 < maskDes.width())
                    tripletList.push_back(Triplet<float>(d, n, -1.0f));

                n = j * maskDes.width() + (i-1);
                if (maskDes.smartAccessor(i-1, j, 0) < 0.5f && i-1 >= 0)
                    tripletList.push_back(Triplet<float>(d, n, -1.0f));

                n = (j+1) * maskDes.width() + i;
                if (maskDes.smartAccessor(i, j+1, 0) < 0.5f && j+1 < maskDes.height())
                    tripletList.push_back(Triplet<float>(d, n, -1.0f));

                n = (j-1) * maskDes.width() + i;
                if (maskDes.smartAccessor(i, j-1, 0) < 0.5f && j-1 >= 0)
                    tripletList.push_back(Triplet<float>(d, n, -1.0f));
            }
            else {
                int d = j * maskDes.width() + i;
                tripletList.push_back(Triplet<float>(d, d, 1.0f));
            }
        }
    }
    A.setFromTriplets(tripletList.begin(), tripletList.end());

    return A;
}


VectorXf getB_2D(const FloatImage &imSrc, const FloatImage &imDes,  const FloatImage &maskSrc, const FloatImage &maskDes, int channel, bool mixGrad) {
    int N = maskDes.width() * maskDes.height(); // CHANGE ME
    VectorXf b(N);

    // find the offset of mask source and mask destination
    int offset_x = 0;
    int offset_y = 0;
    for (int i = 0; i < maskSrc.width(); i++) {
        for (int j = 0; j < maskSrc.height(); j++) {
            if (maskSrc(i, j, channel) < 0.5) {
                offset_x += i;
                offset_y += j;
                goto finish1;
            }
        }
    }
    finish1:
    for (int i = 0; i < maskDes.width(); i++) {
        for (int j = 0; j < maskDes.height(); j++) {
            if (maskDes(i, j, channel) < 0.5) {
                offset_x -= i;
                offset_y -= j;
                goto finish2;
            }
        }
    }
    finish2:

    // build vector b using the gradient
    FloatImage gradientSrc = laplacian(imSrc);

    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            int d = j * maskDes.width() + i;
            if (maskDes(i, j, 0) < 0.5f) { // if is not white

                // add boundary condition
                b(d) = 0.0f;
                // right
                if (i+1 < maskDes.width() && maskDes(i+1, j, 0) > 0.5f)
                    b(d) += imDes.smartAccessor(i+1, j, channel);
                // left
                if (i > 0 && maskDes(i-1, j, 0) > 0.5f)
                    b(d) += imDes.smartAccessor(i-1, j, channel);
                // down
                if (j+1 < maskDes.height() && maskDes(i, j+1, 0) > 0.5f)
                    b(d) += imDes.smartAccessor(i, j+1, channel);
                // up
                if (j > 0 && maskDes(i, j-1, 0) > 0.5f)
                    b(d) += imDes.smartAccessor(i, j-1, channel);

                // add special boundary when the mask is close to edge of image
                if (i == imDes.width()-1)
                    b(d) += imDes.smartAccessor(i+1, j, channel);
                if (i == 0)
                    b(d) += imDes.smartAccessor(i-1, j, channel);
                if (j == imDes.height() - 1)
                    b(d) += imDes.smartAccessor(i, j+1, channel);
                if (j == 0)
                    b(d) += imDes.smartAccessor(i, j-1, channel);

                // no mixed gradient, just use the gradient of source image
                if (!mixGrad) {
                    float srcGrad = gradientSrc.smartAccessor(i+offset_x, j+offset_y, channel);
                    b(d) += srcGrad;
                }

                // mixed gradient, find the max gradient in all four direction
                else {
                    // left gradient
                    if (i > 0) {
                        float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x-1, j+offset_y, channel, true);
                        float desGrad = imDes(i, j, channel) - imDes(i-1, j, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }
                    // right gradient
                    if (i+1 < maskDes.width()) {
                        float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x+1, j+offset_y, channel, true);
                        float desGrad = imDes(i, j, channel) - imDes(i+1, j, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }
                    // up gradient
                    if (j > 0) {
                        float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x, j+offset_y-1, channel, true);
                        float desGrad = imDes(i, j, channel) - imDes(i, j-1, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }
                    // down gradient
                    if (j+1 < maskDes.height()) {
                        float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x, j+offset_y+1, channel, true);
                        float desGrad = imDes(i, j, channel) - imDes(i, j+1, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }
                }
            }

            // if not in the mask, just return the target image color
            else {
                b(d) = imDes(i, j, channel);
            }
        }
    }
    return b;
}


FloatImage solve_2D(const FloatImage &imDes, const SparseMatrix<float> &A, const VectorXf &b) {
    FloatImage result(imDes.width(), imDes.height(), 1);

    // https://eigen.tuxfamily.org/dox/group__TutorialSparse.html
    // https://eigen.tuxfamily.org/dox/group__TopicSparseSystems.html
    SimplicialLDLT<SparseMatrix<float>, Lower, AMDOrdering<int> > solver;
    solver.compute(A);
    VectorXf x = solver.solve(b);

    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            result(j * imDes.width() + i) = x(j * imDes.width() + i);
        }
    }

    return result;
}


///////////////////////////////////////////////////////////////
// texture flattening
//////////////////////////////////////////////////////////////
FloatImage textureFlattening(const FloatImage &im, const FloatImage &mask, const FloatImage &edgeIm, bool isLog){
    
    FloatImage flattenedIm(im), imSrc(im);
    
    if(isLog)
        imSrc = log10FloatImage(im);
    
    // get matrix A, same for all channels
    printf("Matrix A \n");
    SparseMatrix<float> A = getA_2D(mask);
    
    // for each channels, get vector b
    printf("Vector b \n");
    VectorXf br = getB_tf(imSrc, mask, edgeIm, 0);
    VectorXf bg = getB_tf(imSrc, mask, edgeIm, 1);
    VectorXf bb = getB_tf(imSrc, mask, edgeIm, 2);
    
    // solve for x
    printf("Solve xr \n");
    FloatImage xr = solve_2D(im, A, br);
    printf("Solve xg \n");
    FloatImage xg = solve_2D(im, A, bg);
    printf("Solve xb \n");
    FloatImage xb = solve_2D(im, A, bb);
    
    // combine channels
    printf("Combine channels \n");
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            flattenedIm(i, j, 0) = xr(i, j, 0);
            flattenedIm(i, j, 1) = xg(i, j, 0);
            flattenedIm(i, j, 2) = xb(i, j, 0);
        }
    }
    
    if (isLog) {
        return exp10FloatImage(flattenedIm);
    }else
        return flattenedIm;
}


VectorXf getB_tf(const FloatImage &im, const FloatImage &mask, const FloatImage &edgeIm, int channel){
    int N = mask.width() * mask.height(), index = 0;
    VectorXf b(N);
    
    FloatImage gradientIm = laplacian(im);
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            index = j * im.width() + i;

            // add boundary condition
            if (mask(i, j, channel) < 0.5f) {  // if mask(i, j, channel) is not white
                b(index) = 0.0f;
                // right
                if (i+1 < mask.width() && mask(i+1, j, channel) > 0.5f)
                    b(index) += im.smartAccessor(i+1, j, channel);
                // left
                if (i > 0 && mask(i-1, j, 0) > 0.5f)
                    b(index) += im.smartAccessor(i-1, j, channel);
                // down
                if (j+1 < mask.height() && mask(i, j+1, channel) > 0.5f)
                    b(index) += im.smartAccessor(i, j+1, channel);
                // up
                if (j > 0 && mask(i, j-1, channel) > 0.5f)
                    b(index) += im.smartAccessor(i, j-1, channel);

                // add gradient if it is on the boundary of edges
                if (edgeIm(i, j, channel) == 0) { // if edgeIm(i, j, channel) is black
                    // right
                    if (i+1 < edgeIm.width() && edgeIm(i+1, j, channel) == 1)
                        b(index) += im(i, j, channel) - im(i+1, j, channel);
                    // left
                    if (i-1 >= 0 && edgeIm(i-1, j, channel) == 1)
                        b(index) += im(i, j, channel) - im(i-1, j, channel);
                    // down
                    if (j+1 < mask.height() && edgeIm(i, j+1, channel) == 1)
                        b(index) += im(i, j, channel) - im(i, j+1, channel);
                    // up
                    if (j-1 >= 0 && edgeIm(i, j-1, channel) == 1)
                        b(index) += im(i, j, channel) - im(i, j-1, channel);

                // use gradient of original image if it is the edge
                }else
                    b(index) += gradientIm(i, j, channel);

            // if not in the mask, just return the target image color
            }else
                b(index) = im(i, j, channel);
        }
    }
    
    return b;
}


///////////////////////////////////////////////////////////////
// local illumination and color change
//////////////////////////////////////////////////////////////
FloatImage local_changes(const FloatImage &im, const FloatImage &mask, vector<VectorXf> b, bool isLog){
    FloatImage result(im), imSrc(im);
    if(isLog)
        imSrc = log10FloatImage(im);
    
    // get matrix A, same for all channels
    printf("Matrix A \n");
    SparseMatrix<float> A = getA_2D(mask);

    // solve for x
    printf("Solve xr \n");
    FloatImage xr = solve_2D(im, A, b[0]);
    printf("Solve xg \n");
    FloatImage xg = solve_2D(im, A, b[1]);
    printf("Solve xb \n");
    FloatImage xb = solve_2D(im, A, b[2]);
    
    // combine channels
    printf("Combine channels \n");
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            result(i, j, 0) = xr(i, j, 0);
            result(i, j, 1) = xg(i, j, 0);
            result(i, j, 2) = xb(i, j, 0);
        }
    }
    
    if (isLog) {
        return exp10FloatImage(result);
    }else
        return result;
}

VectorXf getB_local_illu(const FloatImage &im, const FloatImage &mask, int channel, float alpha, float beta){
    int N = im.width()*im.height(), index = 0, count = 0;
    VectorXf b(N);
    float gradNorm = 0.0;
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            if (mask(i, j, 0) < 0.5f) {
                float top = j > 0? im(i, j, channel) - im(i, j-1, channel):0;
                float down = j+1 < im.height()? im(i, j, channel) - im(i, j+1, channel):0;
                float left = i > 0? im(i, j, channel) - im(i-1, j, channel):0;
                float right = i+1 < im.width()? im(i, j, channel) - im(i+1, j, channel):0;
                gradNorm += sqrt(top*top + down*down + left*left + right*right);
                count++;
            }
        }
    }
    
    float aver_grad_norm = gradNorm / count;
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            index = j * im.width() + i;
            if (mask(i, j, channel) < 0.5f) {
                b(index) = 0.0f;

                // boundary condition
                // right
                if (i+1 < mask.width() && mask(i+1, j, channel) > 0.5f)
                    b(index) += im(i+1, j, channel);
                // left
                if (i > 0 && mask(i-1, j, 0) > 0.5f)
                    b(index) += im(i-1, j, channel);
                // down
                if (j+1 < mask.height() && mask(i, j+1, channel) > 0.5f)
                    b(index) += im(i, j+1, channel);
                // up
                if (j > 0 && mask(i, j-1, channel) > 0.5f)
                    b(index) += im(i, j-1, channel);
                
                float top = j > 0? im(i, j, channel) - im(i, j-1, channel):0;
                float down = j+1 < im.height()? im(i, j, channel) - im(i, j+1, channel):0;
                float left = i > 0? im(i, j, channel) - im(i-1, j, channel):0;
                float right = i+1 < im.width()? im(i, j, channel) - im(i+1, j, channel):0;

                // add modified gradient
                if(top != 0)
                    b(index) += pow(alpha * aver_grad_norm/abs(top), beta) * top;
                if(down != 0)
                    b(index) += pow(alpha * aver_grad_norm/abs(down), beta) * down;
                if(left != 0)
                    b(index) += pow(alpha * aver_grad_norm/abs(left), beta) * left;
                if(right != 0)
                    b(index) += pow(alpha * aver_grad_norm/abs(right), beta) * right;
                
            }else
                b(index) = im(i, j, channel);
        }
    }
    
    return b;
}




///////////////////////////////////////////////////////////////
// seamless tiling
//////////////////////////////////////////////////////////////

FloatImage seamless_tiling(const FloatImage &im, bool isLog) {
    FloatImage result(im), imSrc(im);

    if(isLog)
        imSrc = log10FloatImage(im);

    // get matrix A, same for all channels
    printf("Matrix A \n");
    SparseMatrix<float> A = getA_2D(FloatImage(im.width(), im.height(), im.channels()));

    // for each channels, get vector b
    printf("Vector b \n");
    VectorXf br = getB_tile(imSrc, 0);
    VectorXf bg = getB_tile(imSrc, 1);
    VectorXf bb = getB_tile(imSrc, 2);

    // solve for x
    printf("Solve xr \n");
    FloatImage xr = solve_2D(im, A, br);
    printf("Solve xg \n");
    FloatImage xg = solve_2D(im, A, bg);
    printf("Solve xb \n");
    FloatImage xb = solve_2D(im, A, bb);

    // combine channels
    printf("Combine channels \n");
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            result(i, j, 0) = xr(i, j, 0);
            result(i, j, 1) = xg(i, j, 0);
            result(i, j, 2) = xb(i, j, 0);
        }
    }

    if (isLog) {
        return exp10FloatImage(result);
    }else
        return result;
}

VectorXf getB_tile(const FloatImage &im, int channel) {
    int N = im.width() * im.height(); // CHANGE ME
    VectorXf b(N);

    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            int index = j * im.width() + i;
            b(index) = 0.0f;

            // left (west)
            if (i == 0) {
                b(index) += im(im.width() - 1, j, channel); // boundary
                b(index) += im(i, j, channel) - (im(0, j, channel) + im(im.width() - 1, j, channel)) / 2.0f;
            }
            else {
                b(index) += im(i, j, channel) - im(i-1, j, channel);
            }

            // up (north)
            if (j == 0) {
                b(index) += im(i, im.height() - 1, channel); // boundary
                b(index) += im(i, j, channel) - (im(i, 0, channel) + im(i, im.height() - 1, channel)) / 2.0f;
            }
            else {
                b(index) += im(i, j, channel) - im(i, j-1, channel);
            }

            // right (east)
            if (i == im.width() - 1) {
                b(index) += im(0, j, channel); // boundary
                b(index) += im(i, j, channel) - (im(0, j, channel) + im(im.width() - 1, j, channel)) / 2.0f;
            }
            else {
                b(index) += im(i, j, channel) - im(i+1, j, channel);
            }

            // down (south)
            if (j == im.height() - 1) {
                b(index) += im(i, 0, channel); // boundary
                b(index) += im(i, j, channel) - (im(i, 0, channel) + im(i, im.height() - 1, channel)) / 2.0f;
            }
            else {
                b(index) += im(i, j, channel) - im(i, j+1, channel);
            }
        }
    }

    return b;
}






///////////////////////////////////////////////////////////////
// utility function
//////////////////////////////////////////////////////////////

// image --> log10FloatImage
FloatImage log10FloatImage(const FloatImage &im) {
    FloatImage log10Image(im);
    float min_non_zero = image_minnonzero(im);
    
    for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            for (int c = 0; c < im.channels(); c++) {
                if (im(x, y, c) == 0) {
                    log10Image(x, y, c) = log10f(min_non_zero);
                }else
                    log10Image(x, y, c) = log10f(im(x, y, c));
            }
        }
    }
    
    return log10Image; // change this
}

// FloatImage --> 10^FloatImage
FloatImage exp10FloatImage(const FloatImage &im) {
    // take an image in log10 domain and transform it back to linear domain.
    // see pow(a, b)
    FloatImage exp10Image(im);
    
    for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            for (int c = 0; c < im.channels(); c++) {
                exp10Image(x, y, c) = pow(10, im(x, y, c));
            }
        }
    }
    
    return exp10Image; // change this
}

// min non-zero pixel value of image
float image_minnonzero(const FloatImage &im) {
    float min_non_zero = INFINITY;
    
    for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            for (int c = 0; c < im.channels(); c++) {
                if (im(x, y, c) < min_non_zero && im(x, y, c) != 0) {
                    min_non_zero = im(x, y, c);
                }
            }
        }
    }
    
    return min_non_zero; // change this
}

// tile the image in a m X n grid
FloatImage tiledImage(const FloatImage &im, int m, int n) {
    FloatImage result(im.width() * m, im.height() * n, im.channels());
    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                result(i, j, c) = im(i%im.width(), j%im.height(), c);
            }
        }
    }
    return result;
}

// laplacian filter
FloatImage laplacian (const FloatImage &im, bool clamp) {
    Filter lapla(3, 3);
    lapla(0,0) =  0.0f; lapla(1,0) = -1.0f; lapla(2,0) =  0.0f;
    lapla(0,1) = -1.0f; lapla(1,1) =  4.0f; lapla(2,1) = -1.0f;
    lapla(0,2) =  0.0f; lapla(1,2) = -1.0f; lapla(2,2) =  0.0f;

    FloatImage imFilter = lapla.Convolve(im, clamp);

    return imFilter;
}


