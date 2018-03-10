# Image Blending
The main implementation of the Poisson image editing is in the "poisson.cpp". <br \>
We also implemented Laplacian blend which is in "laplacianBlend.cpp". <br \>
All source images are in "/data/input/".

## Division of labor
* poisson image blending (Annie)
* texture flattening (Xinbei)
* local illumination and color change (Xinbei)
* seamless tiling (Annie)
* Laplacian blend (Xinbei)

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
