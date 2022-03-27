本次使用的汇编代码借用自一下两位大佬的github:

1. https://github.com/Mengjintao/gflops_benchmark
2. https://github.com/pigirons/cpufp

第一个链接是基于arm平台的测试，源程序只能测试单核，多核测试不了

第二个链接是基于x86平台，正对sse，fma 和avx等指令集有一个全面的测试。同时支持多线程，开箱即用。

做这个小项目的目的是为了整合两者，即实现一个在arm和x86上能对比的测试tools。链接二的大佬的工具里可自动识别x86支持的指令集，并实现"动态"编译，如果只是测试x86平台，推荐直接使用链接二的工具。