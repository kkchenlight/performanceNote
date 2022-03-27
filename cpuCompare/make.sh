###
 # @Author: kkchen
 # @Date: 2022-03-19 11:35:35
 # @LastEditors: kkchen
 # @LastEditTime: 2022-03-27 15:43:28
 # @Email: 1649490996@qq.com
 # @Description: 编译脚本
### 


if [ "$1" = "SSE" ]; then
    g++ -O3 -c cpu_kernelWrap.cpp 
    as -o cpufp_kernelx86_sse.o cpufp_kernel_x86_sse.s
    g++ -O3 main.cpp cpufp_kernelx86_sse.o cpu_kernelWrap.o -D_SSE_ -o cpufp -lpthread 
fi
if [ "$1" = "AVX" ]; then
    g++ -O3 -c cpu_kernelWrap.cpp 
    as -o cpufp_kernel_x86_avx.o cpufp_kernel_x86_avx.s
    g++ main.cpp -o cpufp cpufp_kernel_x86_avx.o cpu_kernelWrap.o -lpthread -D_AVX_F_
fi
if [ "$1" = "FMA" ]; then
    g++ -O3 -c cpu_kernelWrap.cpp 
    as -o cpufp_kernel_x86_fma.o cpufp_kernel_x86_fma.s
    g++ main.cpp cpufp_kernel_x86_fma.o cpu_kernelWrap.o -D_FMA_ -lpthread -o cpufp  -O3
fi
if [ "$1" = "NEON" ]; then
    g++ -O3 -c cpu_kernelWrap.cpp -std=c++11
    as -o cpufp_kenerl_arm_Neon.o cpufp_kenerl_arm_Neon.s 
    g++ main.cpp cpufp_kenerl_arm_Neon.o cpu_kernelWrap.o -D_NEON_ -lpthread -o cpufp  -O3 -std=c++11
fi

