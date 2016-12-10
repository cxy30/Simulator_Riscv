# Simulator_Riscv

### 关于make
	在simulator目录下
	make lab3可编译得到lab3运行trace的程序
	make performance可编译得到第一部分测试程序

### 关于运行程序
	./lab3 trace/1.trace 可运行相关的trace文件
	./performance 可运行测试程序

### 关于测试程序
	    在测试程序的main函数中（performance.cc)可选择执行testL1()，testL2()或testL3(),其中的参数配置不同，三者都会调用Test函数，调用时可设置cacheNum, size等参数。
        另外，运行结果会放在res.csv文件中，是一个latency的矩阵。

### 关于代码文件
	replace_algorithm, prefetch, bypass代码在replace_algorithm.cc, prefetch.cc, bypass.cc中

