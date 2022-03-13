/*
 * @Author: kkchen
 * @Date: 2022-03-13 10:35:36
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-13 15:19:46
 * @Email: 1649490996@qq.com
 * @Description: 本代码主要用于x86跟arm之间的对比
 */

#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
#include<string>
#include<chrono>
#include<thread>
#include<functional>
#ifdef MAKE_SSE
#include<emmintrin.h>
#endif

#ifdef MAKE_NEON
#include<arm_neon.h>
#endif

const float B2YF = 0.114f;
const float G2YF = 0.587f;
const float R2YF = 0.299f;

using namespace std;
using namespace chrono;
using namespace cv;

void timeTest(function<void()>& fun, string& expName){

    auto start = steady_clock::now();
    fun();
    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << expName  << " cost: " << duration.count() / 1000.0 << " ms" << std::endl;
}


void bgr2grayKenerlC(float* ori, float* dst, Size& size){

    for(int i = 0; i < size.height; i++){
        for(int j = 0; j < size.width; j++){
            float* dstPtr = dst + (j + i * size.width);
            float* oriPtr = ori  + (j + i * size.width) * 3;
            dstPtr[0] = (oriPtr[0] * 0.114 + oriPtr[1] * 0.587 + oriPtr[2] * 0.299);
        }
    }
}

void bgr2grayKenerlOpencv(Mat& ori, Mat& dst){

    cvtColor(ori, dst, COLOR_BGR2GRAY);
}

#ifdef MAKE_SSE


void _mm_deinterleave_ps(__m128 & v_r0, __m128 & v_r1, __m128 & v_g0,
                                __m128 & v_g1, __m128 & v_b0, __m128 & v_b1)
{
    __m128 layer1_chunk0 = _mm_unpacklo_ps(v_r0, v_g1);
    __m128 layer1_chunk1 = _mm_unpackhi_ps(v_r0, v_g1);
    __m128 layer1_chunk2 = _mm_unpacklo_ps(v_r1, v_b0);
    __m128 layer1_chunk3 = _mm_unpackhi_ps(v_r1, v_b0);
    __m128 layer1_chunk4 = _mm_unpacklo_ps(v_g0, v_b1);
    __m128 layer1_chunk5 = _mm_unpackhi_ps(v_g0, v_b1);

    __m128 layer2_chunk0 = _mm_unpacklo_ps(layer1_chunk0, layer1_chunk3);
    __m128 layer2_chunk1 = _mm_unpackhi_ps(layer1_chunk0, layer1_chunk3);
    __m128 layer2_chunk2 = _mm_unpacklo_ps(layer1_chunk1, layer1_chunk4);
    __m128 layer2_chunk3 = _mm_unpackhi_ps(layer1_chunk1, layer1_chunk4);
    __m128 layer2_chunk4 = _mm_unpacklo_ps(layer1_chunk2, layer1_chunk5);
    __m128 layer2_chunk5 = _mm_unpackhi_ps(layer1_chunk2, layer1_chunk5);

    v_r0 = _mm_unpacklo_ps(layer2_chunk0, layer2_chunk3);
    v_r1 = _mm_unpackhi_ps(layer2_chunk0, layer2_chunk3);
    v_g0 = _mm_unpacklo_ps(layer2_chunk1, layer2_chunk4);
    v_g1 = _mm_unpackhi_ps(layer2_chunk1, layer2_chunk4);
    v_b0 = _mm_unpacklo_ps(layer2_chunk2, layer2_chunk5);
    v_b1 = _mm_unpackhi_ps(layer2_chunk2, layer2_chunk5);
}

 void process(__m128 v_b, __m128 v_g, __m128 v_r, __m128 v_cb, __m128 v_cg, __m128 v_cr,
                 __m128 & v_gray){

    v_gray = _mm_mul_ps(v_r, v_cr);
    v_gray = _mm_add_ps(v_gray, _mm_mul_ps(v_g, v_cg));
    v_gray = _mm_add_ps(v_gray, _mm_mul_ps(v_b, v_cb));
}


void bgr2grayKenerlSSE4(float* src, float* dst, int height, int width){
    
    int i = 0;
    int n = height * width;
    int scn = 3;
    __m128 v_cb, v_cg, v_cr;
    static const float coeffs[] = { R2YF, G2YF, B2YF };

    v_cb = _mm_set1_ps(coeffs[0]);
    v_cg = _mm_set1_ps(coeffs[1]);
    v_cr = _mm_set1_ps(coeffs[2]);

    float cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];

    for ( ; i <= n - 8; i += 8, src += scn * 8){
        __m128 v_r0 = _mm_loadu_ps(src);
        __m128 v_r1 = _mm_loadu_ps(src + 4);
        __m128 v_g0 = _mm_loadu_ps(src + 8);
        __m128 v_g1 = _mm_loadu_ps(src + 12);
        __m128 v_b0 = _mm_loadu_ps(src + 16);
        __m128 v_b1 = _mm_loadu_ps(src + 20);

        _mm_deinterleave_ps(v_r0, v_r1, v_g0, v_g1, v_b0, v_b1);

        __m128 v_gray0;
        process(v_r0, v_g0, v_b0, v_cb, v_cg, v_cr,
                        v_gray0);

        __m128 v_gray1;
        process(v_r1, v_g1, v_b1, v_cb, v_cg, v_cr,
                        v_gray1);

        _mm_storeu_ps(dst + i, v_gray0);
        _mm_storeu_ps(dst + i + 4, v_gray1);
    }
    /*for ( ; i < n; i++, src += scn)
            dst[i] = src[0]*cb + src[1]*cg + src[2]*cr;*/
}

#endif

#ifdef MAKE_NEON
void bgr2grayKenelNeon(float* src, float* dst, int height, int width){

    int scn = 3;
    int i = 0;
    int n = height * width;
    float32x4_t v_cb, v_cg, v_cr;
    static const float coeffs[] = { R2YF, G2YF, B2YF };
    v_cb = vdupq_n_f32(coeffs[0]);
    v_cg = vdupq_n_f32(coeffs[1]);
    v_cr = vdupq_n_f32(coeffs[2]);


    for ( ; i <= n - 8; i += 8, src += scn * 8){
        float32x4x3_t v_src = vld3q_f32(src);
        vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));

        v_src = vld3q_f32(src + scn * 4);
        vst1q_f32(dst + i + 4, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
    }

    for ( ; i <= n - 4; i += 4, src += scn * 4){
        float32x4x3_t v_src = vld3q_f32(src);
        vst1q_f32(dst + i, vmlaq_f32(vmlaq_f32(vmulq_f32(v_src.val[0], v_cb), v_src.val[1], v_cg), v_src.val[2], v_cr));
    }

    /*for ( ; i < n; i++, src += scn)
        dst[i] = src[0]*cb + src[1]*cg + src[2]*cr;*/
}
#endif

int main(int argc, char* argv[]){

    //read the image
    string imagePath = argv[1];
    Mat oriMat = imread(imagePath);

    // get float Mat
    Mat floatOriMat;
    oriMat.convertTo(floatOriMat, CV_32FC3);

    //get the loop
    int loop = stoi(argv[2]);

    //get the image info
    int height = oriMat.rows;
    int width  = oriMat.cols;
    Size oriSize = Size(width, height);

    //malloc the memory for dstMat;
    //void* dstPtr = malloc(oriSize.area());


    //create the dstMat
    Mat dstMat = Mat(oriSize, CV_32FC1);
    
    //start the kernel;
    function<void()> fun = bind(bgr2grayKenerlC, (float*)floatOriMat.data, (float*)dstMat.data, oriSize);
    string expName = "bgr2grayKenerlC";
    for(int i = 0; i < loop; i++){
        timeTest(fun, expName);
    }
    

    //start the opencvKernel
    /*function<void()> fun1 = bind(bgr2grayKenerlOpencv, oriMat, dstMat);
    string expName1 = "bgr2grayKenerlOpencv";
    for(int i = 0; i < loop; i++){
        timeTest(fun1, expName1);
    }*/
    
    //start Neon kernel
#ifdef MAKE_NEON
    function<void()> fun3 = bind(bgr2grayKenelNeon, (float*)floatOriMat.data, (float*)dstMat.data, oriSize.height, oriSize.width);
    string expName3 = "bgr2grayKenerlNeon";
    for(int i = 0; i < loop; i++){
        timeTest(fun3, expName3);
    }
#endif

#ifdef MAKE_SSE
    vector<thread> threadVec;
    function<void()> fun4 = bind(bgr2grayKenerlSSE4,  (float*)floatOriMat.data, (float*)dstMat.data, oriSize.height, oriSize.width);
    string expName4 = "bgr2grayKenerlSSE4";
    for(int i = 0; i < loop; i++){
        timeTest(fun4, expName4);
    }
#endif

     
    
    //write the gray image
    imwrite("./gray.jpg", dstMat);

    //free the memory
    //free(dstPtr); // 如果把这段内存去构建Mat, 还需要手动释放吗？
}
