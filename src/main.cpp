

#include "filtering.h"
#include "poisson.h"
#include "utils.h"
#include "a2.h"

using namespace std;

void test_2D() {
    const FloatImage imSrc(DATA_DIR "/input/rainbow.jpg");
    const FloatImage imDes(DATA_DIR "/input/museum.jpg");
    const FloatImage maskSrc(DATA_DIR "/input/rainbow_mask.png");
    const FloatImage maskDes(DATA_DIR "/input/museum_mask.png");

//    FloatImage gradientSrc = laplacian(imSrc);
//    FloatImage gradientDes = laplacian(imDes);
//    gradientSrc.write(DATA_DIR "/output/gradientSrc.png");
//    gradientDes.write(DATA_DIR "/output/gradientDes.png");

    bool mix = true;
    bool log = true;
    FloatImage blend = Poisson_2D(imSrc, imDes, maskSrc, maskDes, mix, log);
    blend.write(DATA_DIR "/output/blend_museum_mixLog.png");
}


void test() {
    // https://eigen.tuxfamily.org/dox/group__TopicSparseSystems.html
    Timer timer;
    int N = 3;
    SparseMatrix<float> A(N,N);
    VectorXf b(N);

    ConjugateGradient<SparseMatrix<float>, Lower|Upper> cg;
    timer.reset();
    cg.compute(A);
    VectorXf x = cg.solve(b);

    printf("ConjugateGradient took %3.5f seconds\n", timer.elapsed()/1000.f);
    float relative_error = (A*x - b).norm(); // norm() is L2 norm
    cout << "The relative error is:\n" << relative_error << endl;
}




void testTF(){
    const FloatImage im(DATA_DIR "/input/child.jpg");
    const FloatImage mask(DATA_DIR "/input/child_mask.png");
    FloatImage edgeIm(DATA_DIR "/input/child_edge.png");
//    FloatImage mask = gradientMagnitude(im);
    
//    cout << edgeIm.channels() << endl;
//    
//    for (int i = 0; i < edgeIm.width(); i++) {
//        for (int j = 0; j < edgeIm.height(); j++) {
        
//            cout << edgeIm(i, j, 0) << endl;
//            cout << edgeIm(i, j, 1) << endl;
//            cout << edgeIm(i, j, 2) << endl;
            
//            if(edgeIm(i, j, 0) > 0.1)
//                edgeIm(i, j, 0) = 1;
//            else
//                edgeIm(i, j, 0) = 0;
        
//        }
//    }
    
    FloatImage flattened = textureFlattening(im, mask, edgeIm);
    flattened.write(DATA_DIR "/output/child_flattened.png");
}

void test_illu_change(){
    //local illumination change
    const FloatImage im(DATA_DIR "/input/turkey.png");
    const FloatImage mask(DATA_DIR "/input/turkey_mask2.png");
    
    vector<VectorXf> b;
    
    for (int i = 0; i < 3; i++) {
        b.push_back(getB_local_illu(log10FloatImage(im), mask, i, 0.05, 0.1));
    }
    
    FloatImage illu_changed = local_changes(im, mask, b);
    illu_changed.write(DATA_DIR "/output/turkey_illu_change2.png");
    
}

void test_color_change(){
    //local color change
    const FloatImage im(DATA_DIR "/input/tulip.jpg");
    const FloatImage mask(DATA_DIR "/input/tulip_mask3.png");
    
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
    decolored.write(DATA_DIR "/output/tulip_decolored.png");
    
    //local color change
    FloatImage imSrc(im);
    
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            imSrc(i, j, 0) = im(i, j, 0)*0.5;
            imSrc(i, j, 1) = im(i, j, 1)*2;
            imSrc(i, j, 2) = im(i, j, 2)*2;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        b[i] = getB_2D(imSrc, im, mask, mask, i);
    }
    
    FloatImage local_color_change = local_changes(im, mask, b, false);
    local_color_change.write(DATA_DIR "/output/tulip_color_change.png");
}

void test_tile() {
    const FloatImage im(DATA_DIR "/input/chalk.png");
    FloatImage tileIm = tiledImage(im);
    tileIm.write(DATA_DIR "/output/chalk_original.png");

    FloatImage im2 = seamless_tiling(im, true);
    FloatImage tileIm2 = tiledImage(im2);
    tileIm2.write(DATA_DIR "/output/chalk_seamlessTile.png");
}


int main() {
    cout << "Hello World!" << endl;
    try { test_2D();}   catch(...) {cout << "test_2D Failed!" << endl;}
//    try { testTF();}   catch(...) {cout << "test_tf Failed!" << endl;}
<<<<<<< HEAD
//    try { test_illu_change();}   catch(...) {cout << "test_ill_change Failed!" << endl;}
=======
    try { test_illu_change();}   catch(...) {cout << "test_ill_change Failed!" << endl;}
>>>>>>> 48f7f7ace5e294b0f86b4cb446dceaa38cc87d74
//    try { test_color_change();}   catch(...) {cout << "test_color_change Failed!" << endl;}

//    try { test_tile();}   catch(...) {cout << "test_tile Failed!" << endl;}

//    try { test();}   catch(...) {cout << "test Failed!" << endl;}
    cout << "END" << endl;
}
