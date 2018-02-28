
#include "poisson.h"
#include "filtering.h"

using namespace std;



FloatImage Poisson_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes) {

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
    VectorXf br = getB_2D(imSrc, imDes, maskSrc, maskDes, 0);
    VectorXf bg = getB_2D(imSrc, imDes, maskSrc, maskDes, 1);
    VectorXf bb = getB_2D(imSrc, imDes, maskSrc, maskDes, 2);

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

    // find maskDes != white
    // if in mask, Aij = 4 if i == j
    // Aij = -1 if i is neighbor of j
    // everything else is zero


    // testing purpose, just identity matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j)
                A.coeffRef(i,j) = 1.0f;
        }
    }

    return A;
}


VectorXf getB_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes, int channel) {
    int N = maskDes.width() * maskDes.height(); // CHANGE ME
    VectorXf b(N);

    // find the offset of mask source and mask destination

    // build vector b
    // 1. if maskDes(i, j) != white, add into b
    // 2. b = - 4 imSrc(i+offset,j+offset) + imSrc(i-1+offset, j+offset) + imSrc(i+1+offset, j+offset)
    //                                      + imSrc(i+offset, j-1+offset) + imSrc(i+offset, j+1+offset)
    //        + neighbor imDes(i, j) if neighbor is on the boundary


    // testing purpose, just return the original image
    for (int i = 0; i < imDes.width(); i++) {
        for (int j = 0; j < imDes.height(); j++) {
            b(j * imDes.width() + i) = imDes(i, j, channel);
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

    // CHANGE ME
    // x is only the pixel value inside the mask
    // testing purpose, x = all pixel value
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












