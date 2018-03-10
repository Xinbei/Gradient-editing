

#include "filtering.h"
#include "poisson.h"
#include "utils.h"
#include "a2.h"
#include "laplacianBlend.h"

using namespace std;

void test_2D() {
    const FloatImage imSrc(DATA_DIR "/input/rainbow.jpg");
    const FloatImage imDes(DATA_DIR "/input/library.jpg");
    const FloatImage maskSrc(DATA_DIR "/input/rainbow_mask.png");
    const FloatImage maskDes(DATA_DIR "/input/library_mask.png");

    bool mix = false;
    bool log = false;
    FloatImage blend = Poisson_2D(imSrc, imDes, maskSrc, maskDes, mix, log);
    blend.write(DATA_DIR "/output/blend_library.png");
}


void testTF(){
    const FloatImage im(DATA_DIR "/input/color-plate.png");
    const FloatImage mask(DATA_DIR "/input/color-plate_mask.png");
    FloatImage edgeIm(DATA_DIR "/input/color-plate_edge.png");
    
    FloatImage flattened = textureFlattening(im, mask, edgeIm);
    flattened.write(DATA_DIR "/output/color-plate_flattened.png");
}

void test_illu_change(){
    //local illumination change
    const FloatImage im(DATA_DIR "/input/man_under_exposed.png");
    const FloatImage mask(DATA_DIR "/input/man_under_exposed_mask.png");
    vector<VectorXf> b;
    
    for (int i = 0; i < 3; i++) {
        b.push_back(getB_local_illu(log10FloatImage(im), mask, i, 0.01, 0.2));
    }
    
    FloatImage illu_changed = local_changes(im, mask, b);
    illu_changed.write(DATA_DIR "/output/man_illu_change.png");
}


void test_color_change(){
    //local color change
    const FloatImage im(DATA_DIR "/input/toy.jpg");
    const FloatImage mask(DATA_DIR "/input/toy_mask.png");
    
    vector<VectorXf> b;
    
    //decolorization except selected region
    const FloatImage imGray_one = color2gray(im);
    FloatImage imGray(im);
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            imGray(i, j, 0) = imGray_one(i, j, 0);
            imGray(i, j, 1) = imGray_one(i, j, 0);
            imGray(i, j, 2) = imGray_one(i, j, 0);
        }
    }
    
    for (int i = 0; i < 3; i++) {
        b.push_back(getB_2D(im, imGray, mask, mask, i));
    }

    FloatImage decolored = local_changes(im, mask, b, false);
    decolored.write(DATA_DIR "/output/toy_decolored.png");
    
    //local color change
    FloatImage imSrc(im);
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            imSrc(i, j, 0) = im(i, j, 0)*0.2;
            imSrc(i, j, 1) = im(i, j, 1)*0.2;
            imSrc(i, j, 2) = im(i, j, 2)*2;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        b[i] = getB_2D(imSrc, im, mask, mask, i);
    }
    
    FloatImage local_color_change = local_changes(im, mask, b, false);
    local_color_change.write(DATA_DIR "/output/toy_color_change.png");
}


void test_tile() {
    const FloatImage im(DATA_DIR "/input/tile2.jpg");
    FloatImage tileIm = tiledImage(im);
    tileIm.write(DATA_DIR "/output/tile2_original.png");

    FloatImage im2 = seamless_tiling(im, true);
    FloatImage tileIm2 = tiledImage(im2);
    tileIm2.write(DATA_DIR "/output/tile2_seamlessTile.png");
}


void test_laplacian(){
    const FloatImage imSrc(DATA_DIR "/input/doll_lap.png");
    const FloatImage imDes(DATA_DIR "/input/yard.jpg");
    const FloatImage mask(DATA_DIR "/input/yard_lap_mask.png");
    
    FloatImage output = laplacian_blend(imSrc, imDes, mask);
    output.write(DATA_DIR "/output/lap_blend_yard.png");
}




int main() {
    cout << "Hello World!" << endl;
    try { test_2D();}   catch(...) {cout << "test_2D Failed!" << endl;}
//    try { testTF();}   catch(...) {cout << "test_tf Failed!" << endl;}
//    try { test_illu_change();}   catch(...) {cout << "test_ill_change Failed!" << endl;}
//    try { test_color_change();}   catch(...) {cout << "test_color_change Failed!" << endl;}
//    try { test_tile();}   catch(...) {cout << "test_tile Failed!" << endl;}
//    try { test_laplacian();}   catch(...) {cout << "test_laplacian Failed!" << endl;}
    cout << "END" << endl;
}
