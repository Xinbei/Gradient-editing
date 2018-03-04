
#include "poisson.h"
#include "filtering.h"

using namespace std;



FloatImage Poisson_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes,
                      bool mixGrad) {

//    // chenck mask 1 == mask 2
//    if (maskSrc.width() != maskDes.width() || maskSrc.height() != maskDes.height())
//        throw std::invalid_argument( "The size of two masks does not match." );

    FloatImage result(imDes.width(), imDes.height(), imDes.channels());

    // separate channels
    vector<FloatImage> rgb = {FloatImage(imDes.width(), imDes.height(), 1),
                              FloatImage(imDes.width(), imDes.height(), 1),
                              FloatImage(imDes.width(), imDes.height(), 1)};
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            rgb[0](i, j, 0) = imDes(i, j, 0);
            rgb[1](i, j, 0) = imDes(i, j, 1);
            rgb[2](i, j, 0) = imDes(i, j, 2);
        }
    }

    // get matrix A, same for all channels
    printf("Matrix A \n");
    SparseMatrix<float> A = getA_2D(maskDes);

    // for each channels, get vector b
    printf("Vector b \n");
    VectorXf br = getB_2D(imSrc, imDes, maskSrc, maskDes, 0, mixGrad);
    VectorXf bg = getB_2D(imSrc, imDes, maskSrc, maskDes, 1, mixGrad);
    VectorXf bb = getB_2D(imSrc, imDes, maskSrc, maskDes, 2, mixGrad);

    // solve for x
    printf("Solve xr \n");
    FloatImage xr = solve_2D(imDes, A, br, maskDes);
    printf("Solve xg \n");
    FloatImage xg = solve_2D(imDes, A, bg, maskDes);
    printf("Solve xb \n");
    FloatImage xb = solve_2D(imDes, A, bb, maskDes);

    // combine channels
    printf("Combine channels \n");
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            result(i, j, 0) = xr(i, j, 0);
            result(i, j, 1) = xg(i, j, 0);
            result(i, j, 2) = xb(i, j, 0);
        }
    }

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


VectorXf getB_2D(const FloatImage &imSrc, const FloatImage &imDes,  const FloatImage &maskSrc, const FloatImage &maskDes,
                 int channel, bool mixGrad) {
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
    FloatImage gradientDes = laplacian(imDes);

    // testing purpose, just return the original image
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            int d = j * maskDes.width() + i;
            if (maskDes(i, j, channel) < 0.5f) { // if is not white

                // b = gradient of source image
<<<<<<< HEAD
                float srcGrad = gradientSrc.smartAccessor(i+offset_x, j+offset_y, channel);
                if(mixGrad){
                    float desGrad = gradientDes.smartAccessor(i, j, channel);
                    b(d) = abs(desGrad) > abs(srcGrad) ? desGrad : srcGrad;
                }else
                    b(d) = srcGrad;
=======
//                float srcGrad = gradientSrc.smartAccessor(i+offset_x, j+offset_y, channel);
//                float desGrad = gradientDes.smartAccessor(i, j, channel);
                
//                if (mixGrad) {
//                    b(d) = fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
////                    b(d) = desGrad;
//                }else
//                    b(d) = srcGrad;
                
                if (i > 0) {
                    if (maskDes.smartAccessor(i-1, j, 0) > 0.5f) {
                        b(d) += imDes(i-1, j, channel);
                    }
                    
                    float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x-1, j+offset_y, channel, true);
                    
                    if (mixGrad) {
                        float desGrad = imDes(i, j, channel) - imDes(i-1, j, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }else
                        b(d) += srcGrad;
                }
                
                if (i+1 < maskDes.width()) {
                    if (maskDes.smartAccessor(i+1, j, 0) > 0.5f) {
                        b(d) += imDes(i+1, j, channel);
                    }
                    
                    float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x+1, j+offset_y, channel, true);
                    
                    if (mixGrad) {
                        float desGrad = imDes(i, j, channel) - imDes(i+1, j, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }else
                        b(d) += srcGrad;
                }
                
                if (j > 0) {
                    if (maskDes.smartAccessor(i, j-1, 0) > 0.5f) {
                        b(d) += imDes(i, j-1, channel);
                    }
                    
                    float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x, j+offset_y-1, channel, true);
                    
                    if (mixGrad) {
                        float desGrad = imDes(i, j, channel) - imDes(i, j-1, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }else
                        b(d) += srcGrad;
                }
                
                if (j+1 < maskDes.height()) {
                    if (maskDes.smartAccessor(i, j+1, 0) > 0.5f) {
                        b(d) += imDes(i, j+1, channel);
                    }
                    
                    float srcGrad = imSrc.smartAccessor(i+offset_x, j+offset_y, channel, true) - imSrc.smartAccessor(i+offset_x, j+offset_y+1, channel, true);
                    
                    if (mixGrad) {
                        float desGrad = imDes(i, j, channel) - imDes(i, j+1, channel);
                        b(d) += fabsf(desGrad) > fabsf(srcGrad) ? desGrad:srcGrad;
                    }else
                        b(d) += srcGrad;
                }
>>>>>>> 0dc262dd73ad4273ab16cd0cd8fefec93610eaa2

                // add boundary condition
//                if (maskDes.smartAccessor(i+1, j, 0) > 0.5f && i+1 < maskDes.width())
//                    b(d) += imDes(i+1, j, channel);
//                if (maskDes.smartAccessor(i-1, j, 0) > 0.5f && i-1 >= 0)
//                    b(d) += imDes(i-1, j, channel);
//                if (maskDes.smartAccessor(i, j+1, 0) > 0.5f && j+1 < maskDes.height())
//                    b(d) += imDes(i, j+1, channel);
//                if (maskDes.smartAccessor(i, j-1, 0) > 0.5f && j-1 >= 0)
//                    b(d) += imDes(i, j-1, channel);
            }
            else {
                b(d) = imDes(i, j, channel);
            }
        }
    }
    return b;
}


FloatImage solve_2D(const FloatImage &imDes, const SparseMatrix<float> &A, const VectorXf &b, const FloatImage &maskDes) {
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













FloatImage Poisson_1D(const FloatImage &imSrc, const FloatImage &imDes, int min1, int max1, int min2, int max2) {
    // chenck mask 1 == mask 2
    if (max1 - min1 != max2 - min2)
        throw std::invalid_argument( "The size of two masks does not match." );

    // separate channels
    vector<FloatImage> rgb = {FloatImage(imDes.width(), imDes.height(), 1),
                              FloatImage(imDes.width(), imDes.height(), 1),
                              FloatImage(imDes.width(), imDes.height(), 1)};
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            rgb[0](i, j, 0) = imDes(i, j, 0);
            rgb[1](i, j, 0) = imDes(i, j, 1);
            rgb[2](i, j, 0) = imDes(i, j, 2);
        }
    }


    // get matrix A and vector b
    MatrixXf A = getA_1D(max1, min1);
    vector<VectorXf> b;

    // solve for x
    FloatImage result(imDes.width(), imDes.height(), imDes.channels());

    return result;
}


MatrixXf getA_1D(int min, int max) {
    int N = max - min;
    MatrixXf A(N, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                A(i, j) = 4;
            else if (i == j - 1 || i == j + 1)
                A(i, j) = -2;
            else
                A(i, j) = 0;
        }
    }
    return A;
}


VectorXf getB_1D(const FloatImage &imSrc, const FloatImage &imDes, int min1, int max1, int min2, int max2) {
    int N = max1 - min1;
    VectorXf b(N);

    for (int i = min1; i < max1; i++) {
        b(i-min1) = 4 * imSrc(i) - 2 * imSrc(i-1) - 2 * imSrc(i+1);
        if (i == min1) {
            b(i-min1) += 2 * imDes(i-1);
        }
        if (i == max1 - 1) {
            b(i-min1) += 2 * imDes(i+1);
        }
    }

    return b;
}



FloatImage solve_1D(const FloatImage &imDes, const MatrixXf &A, const VectorXf &b, int min2, int max2) {
    // https://eigen.tuxfamily.org/dox/group__LeastSquares.html
    // https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
    FloatImage result(imDes);
    VectorXf x = A.fullPivLu().solve(b);
    for (int i = min2; i < max2; i++) {
        result(i) = x(i-min2);
    }
    return result;
}









FloatImage laplacian (const FloatImage &im, bool clamp) {
    Filter lapla(3, 3);
    lapla(0,0) =  0.0f; lapla(1,0) = -1.0f; lapla(2,0) =  0.0f;
    lapla(0,1) = -1.0f; lapla(1,1) =  4.0f; lapla(2,1) = -1.0f;
    lapla(0,2) =  0.0f; lapla(1,2) = -1.0f; lapla(2,2) =  0.0f;

    FloatImage imFilter = lapla.Convolve(im, clamp);

    return imFilter;
}





