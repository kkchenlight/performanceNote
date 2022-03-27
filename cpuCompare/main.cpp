/*
 * @Author: kkchen
 * @Date: 2022-03-19 10:37:14
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-27 15:36:47
 * @Email: 1649490996@qq.com
 * @Description: 主要用于不同的cpu对比
 */
#include<thread>
#include<iostream>
#include<chrono>
#include<vector>
#include<string>
#include<functional>
#include "cpufp_kernelWrap.h"
#include "cpufp_kernel.h"

using namespace std;
using namespace chrono;

void startTest(int threadNum){
    
    string expName = "";
    function<void()> fun32;
    long fp32_comp; //定义循环次数
#ifdef _SSE_
        fun32 = bind(cpufp_kernel_x86_sse_fp32);
        expName = "SSEfp32";
        fp32_comp = 0x30000000L * 64;
#endif
        
#ifdef _AVX_F_
        fun32 = bind(cpufp_kernel_x86_avx_fp32);
        expName = "AVXfp32";
        fp32_comp = 0x20000000L * 320;
#endif

#ifdef _FMA_
        fun32 = bind(cpufp_kernel_x86_fma_fp32);
        expName = "FMAfp32";
        fp32_comp =  0x40000000L * 160;
#endif

#ifdef _NEON_
        fun32 = bind(func2, 1e9);
        expName = "Neonfp32";
        fp32_comp = 1e9 * 96;
#endif

    //warming up
    cout << "warming up " << endl;
    vector<thread> threadVec;
    cout << "string = " << expName << endl;

    for(int i = 0; i < threadNum; i++){
        cout << "warming up " << i << endl;
        threadVec.push_back(thread(cpufp_kernelWarp, threadNum, fun32));
    }
    for(auto& it : threadVec){
        it.join();
    }

    //start 
    threadVec.clear();
    auto start = chrono::steady_clock::now();

    for(int i = 0; i < threadNum; i++){
        threadVec.push_back(thread(cpufp_kernelWarp, threadNum, fun32));
    }

    for(auto& it : threadVec){
        it.join();
    }
    
    auto end = chrono::steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    double perf = fp32_comp * threadNum / (duration.count() * 1e-6) * 1e-9;
    cout << expName << " perf: " << perf << " gflops" << endl;
}


int main(int argc, char* argv[]){

    int numThread = atoi(argv[1]);
    startTest(numThread);
    return 0;
}

