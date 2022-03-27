/*
 * @Author: kkchen
 * @Date: 2022-03-19 10:51:46
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-19 10:55:17
 * @Email: 1649490996@qq.com
 * @Description: 对核函数进行包裹，主要是进行绑核操作
 */

#include "cpufp_kernel.h"
#include<functional>
using namespace std;


void cpufp_kernelWarp(int num, function<void()> fun);