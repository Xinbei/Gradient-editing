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

#include "a6.h"
#include <iostream>
#include <math.h>

using namespace std;

// create a new image that is k times bigger than the input by using nearest neighbor interpolation
FloatImage scale(const FloatImage &im, int x, int y) {
    // create new FloatImage that is factor times larger than input image
    FloatImage result(x, y, im.channels());
    // loop over new FloatImage pixels and set appropriate values (smartAccessor is probably overkill here)
    // offset to pixel center
    float factor_x = float(x) / float(im.width());
    float factor_y = float(y) / float(im.height());
    float offset_x = float(result.width())/float(im.width())/2.0f - 0.5f;
    float offset_y = float(result.height())/float(im.height())/2.0f - 0.5f;
    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                result(i, j, c) = interpolateLin(im, (i-offset_x)/factor_x, (j-offset_y)/factor_y, c, true);
            }
        }
    }
    // return new image
    return result;
}

// create a new image that is k times bigger than the input by using nearest neighbor interpolation
FloatImage scaleNN(const FloatImage &im, float factor) {
	// create new FloatImage that is factor times larger than input image
    FloatImage result((int) floor(im.width() * factor), (int) floor(im.height() * factor), im.channels());
	// loop over new FloatImage pixels and set appropriate values (smartAccessor is probably overkill here)
    // offset to pixel center
    float offset_x = float(result.width())/float(im.width())/2.0f - 0.5f;
    float offset_y = float(result.height())/float(im.height())/2.0f - 0.5f;
    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                result(i, j, c) = im((int) round((i-offset_x)/factor), (int) round((j-offset_y)/factor), c);
            }
        }
    }

	// return new image
	return result;
}

// using bilinear interpolation to assign the value of a location from its neighboring pixel values
float interpolateLin(const FloatImage &im, float x, float y, int z, bool clamp) {
	// Hint: use smartAccessor() to handle coordinates outside the image
    float p1 = im.smartAccessor((int)floor(x), (int)floor(y), z, clamp);
    float p2 = im.smartAccessor((int)ceil(x), (int)floor(y), z, clamp);
    float p3 = im.smartAccessor((int)ceil(x), (int)ceil(y), z, clamp);
    float p4 = im.smartAccessor((int)floor(x), (int)ceil(y), z, clamp);

    float tx = x - (int)floor(x);
    float ty = y - (int)floor(y);

    float lerp_x1 = (1-tx) * p1 + p2 * tx;
    float lerp_x2 = (1-tx) * p4 + p3 * tx;
    float lerp_y = (1-ty) * lerp_x1 + lerp_x2 * ty;

	//return final float value
    return lerp_y;
}

float interpolateBicubic(const FloatImage &im, float x, float y, int z, bool clamp) {
    // Hint: use smartAccessor() to handle coordinates outside the image

    // https://en.wikipedia.org/wiki/Bicubic_interpolation
    float p00 = im.smartAccessor((int)floor(x) - 1, (int)floor(y) - 1, z, clamp);
    float p10 = im.smartAccessor((int)floor(x) + 0, (int)floor(y) - 1, z, clamp);
    float p20 = im.smartAccessor((int)floor(x) + 1, (int)floor(y) - 1, z, clamp);
    float p30 = im.smartAccessor((int)floor(x) + 2, (int)floor(y) - 1, z, clamp);

    float p01 = im.smartAccessor((int)floor(x) - 1, (int)floor(y) - 0, z, clamp);
    float p11 = im.smartAccessor((int)floor(x) + 0, (int)floor(y) - 0, z, clamp);
    float p21 = im.smartAccessor((int)floor(x) + 1, (int)floor(y) - 0, z, clamp);
    float p31 = im.smartAccessor((int)floor(x) + 2, (int)floor(y) - 0, z, clamp);

    float p02 = im.smartAccessor((int)floor(x) - 1, (int)floor(y) + 1, z, clamp);
    float p12 = im.smartAccessor((int)floor(x) + 0, (int)floor(y) + 1, z, clamp);
    float p22 = im.smartAccessor((int)floor(x) + 1, (int)floor(y) + 1, z, clamp);
    float p32 = im.smartAccessor((int)floor(x) + 2, (int)floor(y) + 1, z, clamp);

    float p03 = im.smartAccessor((int)floor(x) - 1, (int)floor(y) + 2, z, clamp);
    float p13 = im.smartAccessor((int)floor(x) + 0, (int)floor(y) + 2, z, clamp);
    float p23 = im.smartAccessor((int)floor(x) + 1, (int)floor(y) + 2, z, clamp);
    float p33 = im.smartAccessor((int)floor(x) + 2, (int)floor(y) + 2, z, clamp);

    float tx = x - (int)floor(x);
    float ty = y - (int)floor(y);

    float cubic_x1 = bicubic(p00, p10, p20, p30, tx);
    float cubic_x2 = bicubic(p01, p11, p21, p31, tx);
    float cubic_x3 = bicubic(p02, p12, p22, p32, tx);
    float cubic_x4 = bicubic(p03, p13, p23, p33, tx);

    float cubic_y = bicubic(cubic_x1, cubic_x2, cubic_x3, cubic_x4, ty);

    //return final float value
    return cubic_y;
}


float bicubic(float p0, float p1, float p2, float p3, float t) {
    float a =  0 * p0 + 2 * p1 + 0 * p2 + 0 * p3;
    float b = -1 * p0 + 0 * p1 + 1 * p2 + 0 * p3;
    float c =  2 * p0 - 5 * p1 + 4 * p2 - 1 * p3;
    float d = -1 * p0 + 3 * p1 - 3 * p2 + 1 * p3;
    a /= 2;
    b /= 2;
    c /= 2;
    d /= 2;

    return d*t*t*t + c*t*t + b*t + a;
}



// create a new image that is k times bigger than the input by using bilinear interpolation
FloatImage scaleLin(const FloatImage &im, float factor) {
	// create new FloatImage that is factor times larger than input image
    FloatImage result((int) floor(im.width() * factor), (int) floor(im.height() * factor), im.channels());

    // offset to pixel center
    float offset_x = float(result.width())/float(im.width())/2.0f - 0.5f;
    float offset_y = float(result.height())/float(im.height())/2.0f - 0.5f;

    // loop over new FloatImage pixels and set appropriate values (use interpolateLin())
    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                result(i, j, c) = interpolateLin(im, (i-offset_x)/factor, (j-offset_y)/factor, c, true);
            }
        }
    }

	// return new image
    return result;
}



FloatImage scaleCubic(const FloatImage &im, float factor) {
    // create new FloatImage that is factor times larger than input image
    FloatImage result((int) floor(im.width() * factor), (int) floor(im.height() * factor), im.channels());

    // offset to pixel center
    float offset_x = float(result.width())/float(im.width())/2.0f - 0.5f;
    float offset_y = float(result.height())/float(im.height())/2.0f - 0.5f;

    // loop over new FloatImage pixels and set appropriate values (use interpolateLin())
    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                result(i, j, c) = interpolateBicubic(im, (i-offset_x)/factor, (j-offset_y)/factor, c, true);
            }
        }
    }

    // return new image
    return result;
}

// rotate an image around its center by theta
FloatImage rotate(const FloatImage &im, float theta) {
	//rotate im around its center by theta
    FloatImage result(im.width(), im.height(), im.channels());
    float center_x = float(im.width()) / 2.0f - 0.5f;
    float center_y = float(im.height()) / 2.0f - 0.5f;

    for (int i = 0; i < result.width(); i++) {
        for (int j = 0; j < result.height(); j++) {
            for (int c = 0; c < result.channels(); c++) {
                // translate
                float c_x = i - center_x;
                float c_y = j - center_y;
                // rotate
                float x = cos(theta) * c_x - sin(theta) * c_y;
                float y = sin(theta) * c_x + cos(theta) * c_y;
                // translate
                x += center_x;
                y += center_y;
                // get color
                result(i, j, c) = interpolateLin(im, x, y, c, false);
            }
        }
    }

	// return rotated image
    return result;
}






















