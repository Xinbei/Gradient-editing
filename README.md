# Image Blending
The main implementation of the Poisson image editing is in the "poisson.cpp".
We also implemented Laplacian blend which is in "laplacianBlend.cpp".
All source images are in "/data/input/".

## Division of labor
* Poisson image blending (Annie)
* Texture flattening (Xinbei)
* Local illumination and color change (Xinbei)
* Seamless tiling (Annie)
* Laplacian blend (Xinbei)

## Function description
```
Poisson_2D(FloatImage sourceIm, FloatImage destIm, FloatImage sourceMask, FloatImage destMask, bool isMix, bool isLog)
```
The main function for poisson blending.
```
FloatImage textureFlattening(FloatImage image, FloatImage mask, FloatImage edgeIm, bool isLog)
```
Function for texture flattening.
```
FloatImage local_changes(FloatImage image, FloatImage mask, bool isLog)
```
Function for local illumination and color change.
```
FloatImage seamless_tiling(FloatImage image, FloatImage mask, bool isLog)
```
Function for seamless_tiling.
```
FloatImage laplacian_blend(FloatImage sourceIm, FloatImage destIm, FloatImage mask)
```
The main function for laplacian pyramid blending.
```
FloatImage laplacian_pyramid(FloatImage image, float sigma)
```
Function for calculating laplacian pyramid levels.
```
FloatImage gauss_pyramid(FloatImage image, float sigma)
```
Function for calculating gaussian pyramid levels.
```
FloatImage collapse(vector<FloatImage> im_levels)
```
Function for reconstruct original image.
```
FloatImage downsample(const FloatImage &im, float scale)
```
Linear downsampling
```
FloatImage upsample(const FloatImage &im, int width, int height)
```
Linear upsampling


## How to run the code
* poisson image blending: 
```
test_2D()
```
you need source image, target image, source mask, target mask
* texture flattening: 
```
testTF()
```
you need an image, a mask, and a edge image (obtained using Canny edge detection with Matlab)
* local illumination: 
```
test_illu_change()
```
takes an image and a mask
* local color change: 
```
test_color_change()
```
takes a source image, target color image, source mask, target mask
* seamless tiling: 
```
test_tile()
```
only needs a source image
* laplacian blend
```
test_laplacian()
```
needs a source image, a target image, and a mask.

## Supplement
The mask of poisson image editing need not to be overlap exactly. In other word, you can paste the left top of the source image
to right bottom of the target image, depends on your mask.
However, the mask of laplacian blend has to match. In other word, you can only paste the left top of the source image to the same
location at the target image.

The masks are obtained using Photoshop. Black means in the mask, white means outside of the mask.
