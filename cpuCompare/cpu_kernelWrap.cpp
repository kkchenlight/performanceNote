/*
 * @Author: kkchen
 * @Date: 2022-03-19 10:56:38
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-27 15:46:58
 * @Email: 1649490996@qq.com
 * @Description: 对核函数进行绑核
 */
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <stdint.h>  
#include <sched.h> 
#include <thread>
#include <chrono>
#include <iostream>
#include "./cpufp_kernelWrap.h"
using namespace std;
using namespace chrono;
void cpufp_kernelWarp(int threadNum, function<void()> fun){
    
    cpu_set_t mask;  
    CPU_ZERO(&mask);  

    //绑定线程的cpu
    CPU_SET(threadNum, &mask); 

    if(-1 == pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask))  
    {  
        fprintf(stderr, "pthread_setaffinity_np erro\n");  
        return;
    }

    auto start = steady_clock::now();
    fun();  
    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    cout << "thread id " << threadNum << " end, cost " << duration.count() * 1e-6 << " s" << endl;
}
