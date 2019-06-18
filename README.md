# SourceMonitor
### 1、简介
	SourceMonitor是一个linux平台资源监控库，可以用来监控CPU、内存、存储IO、网络IO等资源的使用情况。该库最突出的特点是以进程为单位对资源进行监控，而且该库提供了简单易用的接口。
	我们之所以提供一个资源监控库而非资源监控工具，主要是考虑到使用的灵活性。用户可以根据自己的需求，借助SourceMonitor库方便的搭建资源监控工具。
### 2、API介绍
	SourceMonitor的工作流程如下图所示：

（1）注册监控参数：registerStatistic(StatisticT statistic)；
	StatisticT是枚举类型，定义了SourceMonitor支持的监控参数。
（2）注册被监控进程：registerPid(unsigned int pid)；
	pid是被监控进程的进程号。
（3）初始化：init()
	初始化并开始监控。
（4）更新：update()
	将监控参数记录到一张表中。
（5）读取监控数据：getStatistic(unsigned int pid,StatisticT statistic)
	读取进程pid的监控参数statistic。
### 3、支持的监控参数
	CpuRatio：		cpu利用率（进程占用cpu时间/墙上时间）
    CpuUsrRatio：	用户态cpu利用率（进程用户态占用cpu时间/墙上时间）
    CpuSysRatio：	内核态cpu利用率（进程内核态占用cpu时间/墙上时间）
    CpuGstRatio,：	虚拟cpu利用率（Guest time of the process）
    CpuWaitRatio：	进程等待运行的时间开销（等待时间/墙上时间）
    CpuIndex：		当前运行进程的cup号

    MemRatio：		进程内存使用率（进程物理内存占用/总物理内存）
    MemRss,：		进程物理内存占用
    MemVsz：		进程虚拟内存占用
    MemMinfltRate,：	进程发生minor faults 的频率
    MemMajfltRate：	进程发生major faults 的频率

    IoRdRate	：		进程硬盘读速率
    IoWrRate：		进程硬盘写速率
    IoCcwrRate：		进程cancelled write的速率
    IoDelay：		进程的block I/O延时

    NetSdRate：		进程网络发送速率（包括TCP、UDP）（要求root权限）
NetRcRate：		进程网络接收速率（包括TCP、UDP）（要求root权限）

### 4、安装
ubuntu：
（1）安装SourceMonitor依赖。
	sudo apt-get install build-essential libpcap-dev cmake
（2）下载SourceMonitor
	git clone https://github.com/zzpp3377/SourceMonitor.git
（3）编译
	cd SourceMonitor && mkdir build && cd build
	cmake ..
	make
	sudo make install
	编译生成一个动态链接库（libSrcMntr.so）和静态链接库(libSrcMntr.a),然后安装到了/usr/lib/下。
	
### 5、示例
	完成安装后，可以进入SourceMonitor/example编译运行示例。
	如示例中所展示的，使用SourceMonitor仅需要#include “sourceMonitor.hpp”，然后按照API介绍中描述的方法使用接口即可。
	编译脚本SourceMonitor/example/compile.sh分别提供了SourceMonitor动态链接库和静态链接库的编译方法。
