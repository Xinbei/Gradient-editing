//
//  laplacianBlend.h
//  final
//
//  Created by Xinbei Fu on 3/8/18.
//
//

#ifndef laplacianBlend_h
#define laplacianBlend_h

#include "floatimage.h"
#include <iostream>
#include <math.h>
#include "filtering.h"
#include <sstream>

using namespace std;

FloatImage laplacian_blend(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &mask);

FloatImage downsample(const FloatImage &im, float scale);
FloatImage upsample(const FloatImage &im, int width, int height);
float interpolateLin(const FloatImage &im, float x, float y, int z, bool clamp);

vector<FloatImage> gauss_pyramid(const FloatImage &im, float sigma);
vector<FloatImage> laplacian_pyramid(const FloatImage &im, float sigma);
FloatImage collapse(vector<FloatImage> im_levels);

#endif /* laplacianBlend_h */
