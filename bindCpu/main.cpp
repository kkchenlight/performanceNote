/*
 * @Author: kkchen
 * @Date: 2022-03-12 10:35:54
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-12 15:05:01
 * @Email: 1649490996@qq.com
 * @Description: file content
 */

#include<iostream>
#include<opencv2/opencv.hpp>
#include<thread>
#include<string>
#include<chrono>
#include<functional>
#include<vector>


#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <stdint.h>  
#include <sched.h>  

using namespace std;
using namespace cv;
using namespace chrono;
using namespace placeholders;

void timeTest(function<void(int)>& fun, int id){
    auto start = steady_clock::now();
    fun(id);
    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << " channel " << id  << " cost: " << duration.count() / 1000.0 << " s" << std::endl;
}

void opencvKernel(Mat& ori, int loop, int id){
    Mat dst;
    for(int i = 0; i < loop; i++){
    auto start   = steady_clock::now(); 
        cvtColor(ori, dst, COLOR_BGR2GRAY);
    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << " channel " << id  << " cost: " << duration.count() / 1000.0 << " s" << std::endl;
    }
}


int main(int argc, char* argv[]){
    
  /*  cpu_set_t get;
    CPU_ZERO(&get);


    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)//设置线程CPU亲和力
    {
        cout << "warning: could not set CPU affinity, continuing...\n" << std::endl;
    }

    if (sched_getaffinity(0, sizeof(get), &get) == -1)//获取线程CPU亲和力
    {
        printf("warning: cound not get thread affinity, continuing...\n");
    }

    if (CPU_ISSET(1, &get)){

        cout << "this thread  " << 1 << " is running processor:" <<  1 << endl;
    }*/
        //read the image;
    string imagPath = argv[1];
    Mat ori = imread(imagPath);

    int loop = stoi(argv[2]);
    int threadNum = stoi(argv[3]);

    //start the kernel
    function<void(int)> fun = bind(opencvKernel, ori, loop, _1);

    //start the thread()
    vector<thread> threadVec;

    auto start   = steady_clock::now();
    for(int i = 0; i < threadNum; i++){
        auto it = thread(std::ref(timeTest), std::ref(fun), i);
        threadVec.push_back(std::move(it));
    }

    for(auto& it : threadVec){
        it.join();
    }

    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << " all thread  cost: " << duration.count() / 1000.0 << " s" << std::endl;
    return 0;
}