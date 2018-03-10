//
//  laplacianBlend.cpp
//  final
//
//  Created by Xinbei Fu on 3/8/18.
//
//

#include <stdio.h>
#include "laplacianBlend.h"

FloatImage laplacian_blend(const FloatImage &imSrc, const FloatImage &imDes, const FloatImage &mask){
    FloatImage result(imDes.width(), imDes.height(), imDes.channels());
    float sigma = 3.0;
    
    //calculates gaussian pyramid for mask
    vector<FloatImage> mask_gauss_py = gauss_pyramid(mask, sigma);
    
    //calculates laplacian pyramids for source image and target image respectively
    vector<FloatImage> imSrc_lap_py = laplacian_pyramid(imSrc, sigma);
    vector<FloatImage> imDes_lap_py = laplacian_pyramid(imDes, sigma);
            
    vector<FloatImage> im_levels;
    
    //blend every level
    for (int i = 0; i < imDes_lap_py.size(); i++)
    {
        im_levels.push_back(mask_gauss_py[i] * imDes_lap_py[i] + (1 - mask_gauss_py[i]) * imSrc_lap_py[i]);
    }
    
    //collapses all levels
    result = collapse(im_levels);
    
    return result;
}

//calculates laplacian pyramid levels
vector<FloatImage> laplacian_pyramid(const FloatImage &im, float sigma){
    vector<FloatImage> result;
    
    //get gaussian pyramid first
    vector<FloatImage> im_gauss_py = gauss_pyramid(im, sigma);
    
    for (int i = 0; i < im_gauss_py.size(); i++) {
        //if it's the last level, just assign it
        if (i == im_gauss_py.size() - 1) {
            result.push_back(im_gauss_py[i]);
        }else{
            //gaussian level i - gaussian level i+1
            result.push_back(im_gauss_py[i] - upsample(im_gauss_py[i+1], im_gauss_py[i].width(), im_gauss_py[i].height()));
        }
    }
    
    return result;
}

//calcuates gaussian pyramid
vector<FloatImage> gauss_pyramid(const FloatImage &im, float sigma){
    vector<FloatImage> result;
    FloatImage im_gauss(im), im_down(im);
    float scale = 2;
    
    int level = INFINITY;
    
    result.push_back(im);
    
    while (level > 2) {
        //do a gaussian blur first
        im_gauss = gaussianBlur_seperable(im_down, sigma);
        
        //then do downsampling
        im_down = downsample(im_gauss, scale);
        result.push_back(im_down);
        
        level = min(im_down.width(), im_down.height());
    }
    
    return result;
}

FloatImage collapse(vector<FloatImage> im_levels){
    FloatImage result(im_levels[im_levels.size()-1]), temp;
    
    for (int i = im_levels.size() - 2; i >= 0; i--) {
        //laplacian pyramid level i + last result
        result = im_levels[i] + upsample(result, im_levels[i].width(), im_levels[i].height());

    }
    
    return result;
}

//linear downsampling
FloatImage downsample(const FloatImage &im, float scale){
    
    int width = floor(im.width()/scale), height = floor(im.height()/scale);
    float sum = 0.0;
    int left = 0, right = 0, up = 0, down = 0;
    
    FloatImage output(width, height, im.channels());
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int c = 0; c < im.channels(); c++) {
                sum = 0;
                
                left = i * scale;
                right = (i + 1) * scale - 1;
                up = j * scale;
                down = (j + 1) * scale - 1;
                
                if (i == width - 1){
                    right = im.width() - 1;
                }
                
                if (j == height - 1){
                    down = im.height() - 1;
                }
                
                for (int l = left; l <= right; l++) {
                    for (int u = up; u <= down; u++) {
                        sum += im(l, u, c);
                    }
                }
                
                output(i, j, c) = sum / ((right - left + 1) * (down - up + 1));
            }
        }
    }
    
    return output;
}

//linear upsampling, same code from assignment6
FloatImage upsample(const FloatImage &im, int width, int height){
    // create new FloatImage that is factor times larger than input image
    FloatImage scaledIm(width, height, im.channels());
    
    // loop over new FloatImage pixels and set appropriate values (use interpolateLin())
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int c = 0; c < im.channels(); c++) {
                scaledIm(x, y, c) = interpolateLin(im, float(x)*(im.width()-1)/(width-1), float(y)*(im.height()-1)/(height-1), c, true);
            }
        }
    }
    
    // return new image
    return scaledIm; // CHANGE ME
}

float interpolateLin(const FloatImage &im, float x, float y, int z, bool clamp)
{
    // Hint: use smartAccessor() to handle coordinates outside the image
    float weightX = x - floor(x), weightY = y - floor(y), U = 0, L = 0;
    
    U = (1 - weightX) * im.smartAccessor(floor(x), floor(y), z, clamp) + weightX * im.smartAccessor(ceil(x), floor(y), z, clamp);
    L = (1 - weightX) * im.smartAccessor(floor(x), ceil(y), z, clamp) + weightX * im.smartAccessor(ceil(x), ceil(y), z, clamp);
    
    //return final float value
    return (1 - weightY) * U + weightY * L; // CHANGE ME
}
