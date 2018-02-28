/*
    CS 89/189 Computational Aspects of Digital Photography C++ basecode.

    Adapted from MIT's 6.815/6.865 basecode, written and designed by:
        Frédo Durand
        Katherine L. Bouman
        Gaurav Chaurasia
        Adrian Vasile Dalca
        Neal Wadhwa

    With additions & modifications by
        Wojciech Jarosz
    for Dartmouth's CS 89/189.
*/

// filtering.cpp
// Assignment 4

#include "filtering.h"
#include "a2.h"
#include <math.h>

using namespace std;

/**************************************************************
 //            IMAGE CONVOLUTION AND FILTERING               //
 *************************************************************/


// convolve an image with a box filter of size k by k
FloatImage boxBlur(const FloatImage &im, const int &k, bool clamp) {
	// create a new empty image
	FloatImage imFilter(im.width(), im.height(), im.channels());

	// convolve the image with the box filter
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            for (int c = 0; c < im.channels(); c++) {
                float sum = 0.0f;
                for (int ki = i - int(k/2); ki <= i + int(k/2); ki++) {
                    for (int kj = j - int(k/2); kj <= j + int(k/2); kj++) {
                        sum += im.smartAccessor(ki, kj, c, clamp);
                    }
                }
                imFilter(i, j, c) = sum / (k*k);
            }
        }
    }

	return imFilter;
}

// reimeplement the box filter using the filter class.
// check that your results math those in the previous function "boxBlur"
FloatImage boxBlur_filterClass(const FloatImage &im, const int &k, bool clamp) {
	// use Convolve() to apply convolution
    Filter boxBlur(k, k);
    for (int x = 0; x < k; x++) {
        for (int y = 0; y < k; y++) {
            boxBlur(x,y) = 1.0f / (k * k);
        }
    }
    FloatImage imFilter = boxBlur.Convolve(im, clamp);

	return imFilter;
}

// uses a Sobel kernel to compute the horizontal and vertical
// components of the gradient of an image and returns the gradient magnitude.
FloatImage gradientMagnitude(const FloatImage &im, bool clamp) {
	// sobel filtering in x direction
    Filter sobelX(3, 3);
    sobelX(0,0) = -1.0f; sobelX(1,0) = 0.0f; sobelX(2,0) = 1.0f;
    sobelX(0,1) = -2.0f; sobelX(1,1) = 0.0f; sobelX(2,1) = 2.0f;
    sobelX(0,2) = -1.0f; sobelX(1,2) = 0.0f; sobelX(2,2) = 1.0f;

	// sobel filtering in y direction
    Filter sobelY(3, 3);
    sobelY(0,0) = -1.0f; sobelY(1,0) = -2.0f; sobelY(2,0) = -1.0f;
    sobelY(0,1) =  0.0f; sobelY(1,1) =  0.0f; sobelY(2,1) =  0.0f;
    sobelY(0,2) =  1.0f; sobelY(1,2) =  2.0f; sobelY(2,2) =  1.0f;

	// compute squared magnitude
    FloatImage imFilterX = sobelX.Convolve(im, clamp);
    FloatImage imFilterY = sobelY.Convolve(im, clamp);

    FloatImage imFilter = FloatImage(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            for (int c = 0; c < im.channels(); c++) {
                imFilter(i, j, c) = sqrtf(powf(imFilterX(i, j, c), 2) + powf(imFilterY(i, j, c), 2));
            }
        }
    }

	return imFilter;
}

// create a vector containing the normalized values in a 1D Gaussian filter
vector<float> gauss1DFilterValues(float sigma, float truncate) {
	// calculate the size of the filter
    int k = 1 + 2 * int(ceil(sigma * truncate));
    int center = k / 2;

	// compute the un-normalized value of the gaussian
    vector <float> gaussVector;
    float sum = 0.0f;
    for (int i = 0; i < k; i++) {
        float g =  exp(- (powf(center - i, 2) / (2 * sigma * sigma)));
        gaussVector.push_back(g);
        sum += g;
    }

	// normalize
    for (int i = 0; i < k; i++) {
        gaussVector[i] /= sum;
    }

	return gaussVector;
}

// blur across the rows of an image
FloatImage gaussianBlur_horizontal(const FloatImage &im, float sigma, float truncate, bool clamp) {
	// filter in the x direction
    int k = 1 + 2 * int(ceil(sigma * truncate));
    Filter gauss(k, 1);
    vector <float> gaussVector = gauss1DFilterValues(sigma, truncate);
    for (int x = 0; x < k; x++) {
        gauss(x,0) = gaussVector[x];
    }

    FloatImage imFilter = gauss.Convolve(im, clamp);
	return imFilter;
}

// create a vector containing the normalized values in a 2D Gaussian filter
vector<float> gauss2DFilterValues(float sigma, float truncate) {
	// compute the filter size
    int k = 1 + 2 * int(ceil(sigma * truncate));
    int center = k / 2;

	// compute the unnormalized value of the gaussian and put it in a row-major vector
    vector <float> gaussVector;
    float sum = 0.0f;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            float r = sqrt(powf(center - i, 2) + powf(center - j, 2));
            float g =  exp(- (powf(r, 2) / (2 * sigma * sigma)));
            gaussVector.push_back(g);
            sum += g;
        }
    }

	// normalize
    for (int i = 0; i < k*k; i++) {
        gaussVector[i] /= sum;
    }

	return gaussVector;
}

// Blur an image with a full  full 2D rotationally symmetric Gaussian kernel
FloatImage gaussianBlur_2D(const FloatImage &im, float sigma, float truncate, bool clamp) {
	// blur using a 2D gaussian filter (use gauss2DFilterValues())
    int k = 1 + 2 * int(ceil(sigma * truncate));
    Filter gauss(k, k);
    vector <float> gaussVector = gauss2DFilterValues(sigma, truncate);
    for (int x = 0; x < k; x++) {
        for (int y = 0; y < k; y++) {
            gauss(x,y) = gaussVector[x + y * k];
        }
    }

    FloatImage imFilter = gauss.Convolve(im, clamp);
    return imFilter;
}

// Use principles of seperabiltity to blur an image using 2 1D Gaussian Filters
FloatImage gaussianBlur_seperable(const FloatImage &im, float sigma, float truncate, bool clamp) {
	// blur using 2, 1D filters in the x and y directions
    int k = 1 + 2 * int(ceil(sigma * truncate));
    Filter gaussX(k, 1);
    Filter gaussY(1, k);
    vector <float> gaussVector = gauss1DFilterValues(sigma, truncate);
    for (int i = 0; i < k; i++) {
        gaussX(i,0) = gaussVector[i];
        gaussY(0,i) = gaussVector[i];
    }

    FloatImage imFilter = gaussX.Convolve(im, clamp);
    imFilter = gaussY.Convolve(imFilter, clamp);

	return imFilter;
}

// sharpen an image
FloatImage unsharpMask(const FloatImage &im, float sigma, float truncate, float strength, bool clamp) {
	// get the low pass image and subtract it from the original image to get the high pass image
    FloatImage lowpassImg = gaussianBlur_seperable(im, sigma, truncate, clamp);
    FloatImage highpassImg = im - lowpassImg;
    FloatImage imFilter = im + strength * highpassImg;
	return imFilter;
}

// Denoise an image using bilateral filtering
FloatImage bilateral(const FloatImage &im, float sigmaRange, float sigmaDomain, float truncateDomain, bool clamp) {

    // calculate the filter size
    int k = 1 + 2 * int(ceil(sigmaDomain * truncateDomain));

	// for every pixel (x,y) in the image set value to weighted sum of values in the filter region
    FloatImage imFilter(im.width(), im.height(), im.channels());

    for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            for (int c = 0; c < im.channels(); c++) {
                float norm = 0.0f;
                for (int kx = 0; kx < k; kx++) {
                    for (int ky = 0; ky < k; ky++) {

                        int x2 = x + int(k/2) - kx;
                        int y2 = y + int(k/2) - ky;

                        float r1 = sqrt(powf(int(k/2) - kx, 2) + powf(int(k/2) - ky, 2));
                        float gaussDomain =  exp(- (powf(r1, 2) / (2 * sigmaDomain * sigmaDomain)));

                        float r2 = 0.0f;
                        for (int c2 = 0; c2 < im.channels(); c2++) {
                            r2 += powf(im.smartAccessor(x, y, c2, clamp) - im.smartAccessor(x2, y2, c2, clamp), 2);
                        }
                        r2 = sqrtf(r2);

                        float gaussRange = exp(- (powf(r2, 2) / (2 * sigmaRange * sigmaRange)));

                        float gauss = gaussDomain * gaussRange;
                        imFilter(x, y, c) += gauss * im.smartAccessor(x2, y2, c, clamp);
                        norm += gauss;
                    }
                }
                imFilter(x, y, c) /= norm;
            }
        }
    }

	return imFilter;
}

// Bilaterial Filter an image seperately for the Y and UV components of an image
FloatImage bilaYUV(const FloatImage &im,
                   float sigmaRange, float sigmaY, float sigmaUV,
                   float truncateDomain, bool clamp) {
    // convert from RGB to YUV
    FloatImage yuv = rgb2yuv(im);

    FloatImage y(im.width(), im.height(), 1);
    FloatImage uv(im.width(), im.height(), 2);

    // denoise Y and UV channels using different domain sigmas
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            y(i, j, 0) = yuv(i, j, 0);
            uv(i, j, 0) = yuv(i, j, 1);
            uv(i, j, 1) = yuv(i, j, 2);
        }
    }

    y = bilateral(y, sigmaRange, sigmaY, truncateDomain, clamp);
    uv = bilateral(uv, sigmaRange, sigmaUV, truncateDomain, clamp);

    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            yuv(i, j, 0) = y(i, j, 0);
            yuv(i, j, 1) = uv(i, j, 0);
            yuv(i, j, 2) = uv(i, j, 1);
        }
    }

	// convert from YUV back to RGB
    FloatImage imFilter = yuv2rgb(yuv);

	return imFilter;
}


/**************************************************************
 //                      EXTRA FUNCTIONS                  //
 *************************************************************/
FloatImage boxBlur_filterClass_separable(const FloatImage &im, const int &k, bool clamp) {
    Filter boxBlurX(k, 1);
    Filter boxBlurY(1, k);
    for (int i = 0; i < k; i++) {
        boxBlurX(i, 0) = 1.0f / k;
        boxBlurY(0, i) = 1.0f / k;
    }

    FloatImage imFilter = boxBlurX.Convolve(im, clamp);
    imFilter = boxBlurY.Convolve(imFilter, clamp);

    return imFilter;
}

FloatImage boxBlur_filterClass_incremental(const FloatImage &im, const int &k, bool clamp) {
    FloatImage imFilter(im.width(), im.height(), im.channels());

    for (int c = 0; c < im.channels(); c++) {
        for (int i = 0; i < im.width(); i++) {
            float sum = 0.0f;
            for (int ki = i - int(k/2); ki <= i + int(k/2); ki++) {
                for (int kj = -1 - int(k/2); kj <= -1 + int(k/2); kj++) {
                    sum += im.smartAccessor(ki, kj, c, clamp);
                }
            }
            for (int j = 0; j < im.height(); j++) {
                for (int ki = i - int(k/2); ki <= i + int(k/2); ki++) {
                    sum -= im.smartAccessor(ki, j - int(k/2) - 1, c, clamp);
                    sum += im.smartAccessor(ki, j + int(k/2), c, clamp);
                }
                imFilter(i, j, c) = sum / (k*k);
            }
        }
    }
    return imFilter;
}

FloatImage boxBlur_filterClass_sNi(const FloatImage &im, const int &k, bool clamp) {

    FloatImage imFilterH(im.width(), im.height(), im.channels());
    FloatImage imFilter(im.width(), im.height(), im.channels());

    for (int c = 0; c < im.channels(); c++) {

        // horizontal box blur
        for (int i = 0; i < im.width(); i++) {
            float sum = 0.0f;
            for (int kj = -1 - int(k/2); kj <= -1 + int(k/2); kj++) {
                sum += im.smartAccessor(i, kj, c, clamp);
            }
            for (int j = 0; j < im.height(); j++) {
                sum -= im.smartAccessor(i, j - int(k/2) - 1, c, clamp);
                sum += im.smartAccessor(i, j + int(k/2), c, clamp);
                imFilterH(i, j, c) = sum / k;
            }
        }

        // vertical box blur
        for (int j = 0; j < im.height(); j++) {
            float sum = 0.0f;
            for (int ki = -1 - int(k/2); ki <= -1 + int(k/2); ki++) {
                sum += imFilterH.smartAccessor(ki, j, c, clamp);
            }
            for (int i = 0; i < im.width(); i++) {
                sum -= imFilterH.smartAccessor(i - int(k/2) - 1, j, c, clamp);
                sum += imFilterH.smartAccessor(i + int(k/2), j, c, clamp);
                imFilter(i, j, c) = sum / k;
            }
        }
    }

    return imFilter;
}

// Fast Gaussian filter approximation using repeated fast box blurs
FloatImage gaussianBlur_box(const FloatImage &im, int k, float sigma, bool clamp) {
    FloatImage imFilter(im);

    // http://fiveko.com/tutorials/image-processing/image-box-blur/
    // number of iterations
    float n = floor((12 * sigma * sigma) / (k * k -1));

    for (int i = 0; i < n; i++) {
        imFilter = boxBlur_filterClass_sNi(imFilter, k, clamp);
    }
    return imFilter;
}


FloatImage miniaturize(const FloatImage &im, float strength, int focal, float sigma, float truncate, bool clamp) {
//    int k = 1 + 2 * int(ceil(sigma * truncate));
//    // for every pixel (x,y) in the image set value to weighted sum of values in the filter region
//    FloatImage imFilter(im.width(), im.height(), im.channels());
//
//    for (int x = 0; x < im.width(); x++) {
//        for (int y = 0; y < im.height(); y++) {
//
//            for (int c = 0; c < im.channels(); c++) {
//                float norm = 0.0f;
//                for (int kx = 0; kx < k; kx++) {
//                    for (int ky = 0; ky < k; ky++) {
//
//                        int x2 = x + int(k/2) - kx;
//                        int y2 = y + int(k/2) - ky;
//
////                        float weight = max(1.0f, 5.0f * (1.0f - fabs(y - im.height()/2.0f) / (im.height()/2.0f)));
//                        float weight = fmax(1.0f, strength * (1.0f - abs(y - focal) / (im.height()/2.0f)));
//
//                        float r = sqrt(powf(int(k/2) - kx, 2) + powf(int(k/2) - ky, 2)) * weight;
//                        float gauss =  exp(- (powf(r, 2) / (2 * sigma * sigma)));
//
//                        imFilter(x, y, c) += gauss * im.smartAccessor(x2, y2, c, clamp);
//                        norm += gauss;
//                    }
//                }
//                imFilter(x, y, c) /= norm;
//            }
//        }
//    }

    FloatImage imFilter = gaussianBlur_seperable(im, sigma, truncate, clamp);
    for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            float weight = fmin(1.0f, strength * abs(y - focal) / (im.height() - focal));
            for (int c = 0; c < im.channels(); c++) {
                imFilter(x, y, c) = weight * imFilter(x, y, c) + (1 - weight) * im(x, y, c);
            }
        }
    }
    return imFilter;
}


/**************************************************************
 //                 FILTER CLASS FUNCTIONS                  //
 *************************************************************/


// write a convolution function for the filter class
FloatImage Filter::Convolve(const FloatImage &im, bool clamp) const {
	FloatImage imFilter(im.width(), im.height(), im.channels());

	// implement convultion
	// Hint: use use Filter::operator()(x, y) to access (x,y) kernel location
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            for (int c = 0; c < im.channels(); c++) {
                for (int x = width - 1; x >= 0; x--) {
                    for (int y = height - 1; y >= 0; y--) {
                        int ki = i + int(width/2) - x;
                        int kj = j + int(height/2) - y;
                        imFilter(i, j, c) += im.smartAccessor(ki, kj, c, clamp) * operator()(x, y);
                    }
                }
            }
        }
    }

	return imFilter;
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
FloatImage impulseImg(const int &k)
{
	// initlize a kxkx1 image of all 0's
	FloatImage impulse(k, k, 1);

	// set the center pixel to have intensity 1
	int center = floor(k / 2);
	impulse(center, center, 0) = 1;

	return impulse;
}

Filter::Filter(const vector<float> &fData, const int &fWidth, const int &fHeight)
{
	// TODO: check that width*height = length of filterVals and that width and height are odd
	kernel = fData;
	width = fWidth;
	height = fHeight;

}

Filter::Filter(const int &fWidth, const int &fHeight)
{
	width = fWidth;
	height = fHeight;
	kernel = std::vector<float>(width * height, 0);
}

const float &Filter::operator()(int x, int y) const
{
	if (x < 0 || x >= width)
		throw OutOfBoundsException();
	if (y < 0 || y >= height)
		throw OutOfBoundsException();

	return kernel[x + y * width];
}

float &Filter::operator()(int x, int y)
{
	if (x < 0 || x >= width)
		throw OutOfBoundsException();
	if (y < 0 || y >= height)
		throw OutOfBoundsException();

	return kernel[x + y * width];
}
Filter::~Filter() {}
