The main implementation of the Poisson image editing is in the "poisson.cpp".
We also implemented Laplacian blend which is in "laplacianBlend.cpp".
All source images are in "/data/input/".


Division of labor :
1. Poisson image blending (Annie)
2. Texture flattening (Xinbei)
3. Local illumination and color change (Xinbei)
4. Seamless tiling (Annie)
5. Laplacian blend (Xinbei)


To run the code:
Comment out the function in "main.cpp"
1. poisson image blending: test_2D(), you need source image, target image, source mask, target mask
2. texture flattening: testTF(), you need an image, a mask, and a edge image (obtained using Canny edge detection with Matlab)
3. local illumination: test_illu_change(), an image and a mask
4. local color change: test_color_change(), a source image, target color image, source mask, target mask
5. seamless tiling: test_tile(), only need a source image
6. laplacian blend: test_laplacian(), need a source image, a target image, and a mask.

The mask of poisson image editing need not to be overlap exactly. In other word, you can paste the left top of the source image
to right bottom of the target image, depends on your mask.
However, the mask of laplacian blend has to match. In other word, you can only paste the left top of the source image to the same
location at the target image.

The masks are obtained using Photoshop. Black means in the mask, white means outside of the mask.
