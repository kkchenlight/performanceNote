/*
 * @Author: kkchen
 * @Date: 2022-03-27 13:54:12
 * @LastEditors: kkchen
 * @LastEditTime: 2022-03-27 15:46:24
 * @Email: 1649490996@qq.com
 * @Description: file content
 */
#ifndef _CPUFP_X86_H
#define _CPUFP_X86_H

extern "C" void cpufp_kernel_x86_sse_fp32();
extern "C" void cpufp_kernel_x86_sse_fp64();

extern "C" void cpufp_kernel_x86_avx_fp32();
extern "C" void cpufp_kernel_x86_avx_fp64();

extern "C" void cpufp_kernel_x86_fma_fp32();
extern "C" void cpufp_kernel_x86_fma_fp64();

extern "C" void cpufp_kernel_x86_avx512f_fp32();
extern "C" void cpufp_kernel_x86_avx512f_fp64();

extern "C" void cpufp_kernel_x86_avx512_vnni_8b();
extern "C" void cpufp_kernel_x86_avx512_vnni_8b();

extern "C" void func2(int loop);

#endif