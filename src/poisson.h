#ifndef FINAL_POISSON_H
#define FINAL_POISSON_H

#include "floatimage.h"
#include <iostream>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include<Eigen/IterativeLinearSolvers>

using namespace std;
using namespace Eigen;


FloatImage laplacian (const FloatImage &im, bool clamp=true);


// 2D case
FloatImage Poisson_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes, bool mixGrad=false);
SparseMatrix<float> getA_2D(const FloatImage &maskDes);
VectorXf getB_2D(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &maskSrc, const FloatImage &maskDes, int channel, bool mixGrad=false);
FloatImage solve_2D(const FloatImage &imDes, const SparseMatrix<float> &A, const VectorXf &b, const FloatImage &maskDes);


// 1D case
FloatImage Poisson_1D(const FloatImage &imSrc, const FloatImage &imDes, int min1, int max1, int min2, int max2);
MatrixXf getA_1D(int min, int max);
VectorXf getB_1D(const FloatImage &imSrc, const FloatImage &imDes, int min1, int max1, int min2, int max2);
FloatImage solve_1D(const FloatImage &imDes, const MatrixXf &A, const VectorXf &b, int min2, int max2);



#endif
