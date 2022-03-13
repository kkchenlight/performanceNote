/*
 * @Author: kkchen
 * @Date: 2022-03-12 10:35:54
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-12 18:07:00
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
    std::cout << " channel " << id  << " cost: " << duration.count() / 1000.0 << " ms" << std::endl;
}

void opencvKernel(Mat& ori, int loop, int id){

    cpu_set_t mask;  
    CPU_ZERO(&mask);  
    cpu_set_t get;  
    CPU_ZERO(&get);  
     

    //绑定线程的cpu
    //CPU_SET(id,&mask); 
    /*("thread %u, i = %d\n", pthread_self(), id);  
    if(-1 == pthread_setaffinity_np(pthread_self() ,sizeof(mask),&mask))  
    {  
        fprintf(stderr, "pthread_setaffinity_np erro\n");  
        return;
    } */ 
    //查询线程所在的cpu
    if (sched_getaffinity(0, sizeof(get), &get) == -1)//获取线程CPU亲和力
    {
        printf("warning: cound not get thread affinity, continuing...\n");
    }
    
    for(int i = 0; i < 64; i++){
      if (CPU_ISSET(i, &get)){

        cout << "this thread  " << id << " is running processor:" <<  i << endl;
        }
    }
  
 
    Mat dst;
    for(int i = 0; i < loop; i++){
    auto start   = steady_clock::now(); 
        cvtColor(ori, dst, COLOR_BGR2GRAY);
    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << " channel kernel" << id  << " cost: " << duration.count() / 1000.0 << " ms" << std::endl;
    }
}


int main(int argc, char* argv[]){
    //给主进程绑定cpu
   /* cpu_set_t get;
    CPU_ZERO(&get);


    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(40, &mask);

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
    //fun(1);

    auto end   = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << " all thread  cost: " << duration.count() / 1000.0 << " ms" << std::endl;
    return 0;
}