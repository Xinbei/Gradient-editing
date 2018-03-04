

#include "filtering.h"
#include "poisson.h"
#include "utils.h"

using namespace std;


void test_1D() {
    FloatImage imgSource(8, 1, 1);
    imgSource(0, 0, 0) = 4;
    imgSource(1, 0, 0) = 3;
    imgSource(2, 0, 0) = 4;
    imgSource(3, 0, 0) = 3;
    imgSource(4, 0, 0) = 5;
    imgSource(5, 0, 0) = 4;
    imgSource(6, 0, 0) = 3;
    imgSource(7, 0, 0) = 2;

    FloatImage imgTarget(8, 1, 1);
    imgTarget(0, 0, 0) = 3;
    imgTarget(1, 0, 0) = 6;
    imgTarget(2, 0, 0) = 0;
    imgTarget(3, 0, 0) = 0;
    imgTarget(4, 0, 0) = 0;
    imgTarget(5, 0, 0) = 0;
    imgTarget(6, 0, 0) = 1;
    imgTarget(7, 0, 0) = 2;

    imgSource.write(DATA_DIR "/output/test1d_img.png");

    MatrixXf A = getA_1D(2, 6);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f \t", A(i, j));
        }
        printf("\n");
    }

    printf("\n");

    VectorXf b = getB_1D(imgSource,imgTarget, 2, 6, 2, 6);
    for (int i = 0; i < 4; i++) {
        printf("%f \t", b(i));
    }
    printf("\n");

    FloatImage imgResult = solve_1D(imgTarget, A, b, 2, 6);
    for (int i = 0; i < imgResult.size(); i++) {
        printf("%f \t", imgResult(i));
    }
}



void test_2D() {
    const FloatImage imSrc(DATA_DIR "/input/animations_small.png");
    const FloatImage imDes(DATA_DIR "/input/wooden_small.png");
    const FloatImage maskSrc(DATA_DIR "/input/animations_small_mask.png");
    const FloatImage maskDes(DATA_DIR "/input/wooden_small_mask.png");

//    FloatImage gradientSrc = laplacian(imSrc);
//    FloatImage gradientDes = laplacian(imDes);
//    gradientSrc.write(DATA_DIR "/output/gradientSrc.png");
//    gradientDes.write(DATA_DIR "/output/gradientDes.png");

    FloatImage blend = Poisson_2D(imSrc, imDes, maskSrc, maskDes, false);
    blend.write(DATA_DIR "/output/blend_wooden.png");
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
    const FloatImage im(DATA_DIR "/input/ha-18.jpg");
    const FloatImage mask(DATA_DIR "/input/ha-18-mask.jpg");
//    FloatImage mask = gradientMagnitude(im);
//    FloatImage test(mask.width(), mask.height(), 1);
    
//    for (int i = 0; i < mask.width(); i++) {
//        for (int j = 0; j < mask.height(); j++) {
//            if(mask(i, j, 0) > 0.3)
//                test(i, j, 0) = 1;
//            else
//                test(i, j, 0) = 0;
//        }
//    }
    
//    test.write(DATA_DIR "/output/ha-18-mask.png");
    
    FloatImage flattened = textureFlattening(im,mask);
    flattened.write(DATA_DIR "/output/ha-18-flattened.png");
}




int main() {
    cout << "Hello World!" << endl;
//    try { test_1D();}   catch(...) {cout << "test_1D Failed!" << endl;}
//    try { test_2D();}   catch(...) {cout << "test_2D Failed!" << endl;}
    try { testTF();}   catch(...) {cout << "test_tf Failed!" << endl;}

//    try { test();}   catch(...) {cout << "test Failed!" << endl;}
    cout << "END" << endl;
}
