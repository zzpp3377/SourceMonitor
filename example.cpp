/*
 * example.cpp
 *
 * Copyright (c) 2019 Zhang Peng
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *USA.
 *
 */

#include "sourceMonitor.hpp"
#include "stdio.h"
#include "stdlib.h"

#include <thread>    
#include <chrono>  

#include <iostream>

void test1(int count, int interval, int pid){
    SourceMonitor sourceMonitor;
    sourceMonitor.registerStatistic(CpuRatio);
    sourceMonitor.registerStatistic(MemRss);
    sourceMonitor.registerStatistic(MemVsz);

    sourceMonitor.registerPid(pid);
    sourceMonitor.init();
    printf("cpuRatio\trss\tvsz\n");
    for(int i=0;i<count;i++){
        std::this_thread::sleep_for (std::chrono::seconds(interval));
        sourceMonitor.update();
        double cpuRatio=sourceMonitor.getStatistic(pid,CpuRatio);
        double rss=sourceMonitor.getStatistic(pid,MemRss);
        double vsz=sourceMonitor.getStatistic(pid,MemVsz);
        printf("%lf\t%lf\t%lf\n",cpuRatio,rss,vsz);
    }
}

void test2(int count, int interval, int pid){
    SourceMonitor sourceMonitor;
    sourceMonitor.registerStatistic(MemMinfltRate);
    sourceMonitor.registerStatistic(MemMajfltRate);
    sourceMonitor.registerStatistic(MemVsz);
    sourceMonitor.registerStatistic(MemRss);
    sourceMonitor.registerStatistic(MemRatio);

    sourceMonitor.registerPid(pid);
    sourceMonitor.init();
    printf("minflt/s\tmajflt/s\tvsz\trss\tMemRatio\n");
    for(int i=0;i<count;i++){
        std::this_thread::sleep_for (std::chrono::seconds(interval));
        sourceMonitor.update();
        double memMinfltRate=sourceMonitor.getStatistic(pid,MemMinfltRate);
        double memMajfltRate=sourceMonitor.getStatistic(pid,MemMajfltRate);
        double rss=sourceMonitor.getStatistic(pid,MemRss);
        double vsz=sourceMonitor.getStatistic(pid,MemVsz);
        double memRatio=sourceMonitor.getStatistic(pid,MemRatio);
        printf("%lf\t%lf\t%lf\t%lf\t%lf\n",memMinfltRate,memMajfltRate,vsz,rss,memRatio);
    }    
}


void test3(int count, int interval, int pid){
    SourceMonitor sourceMonitor;
    sourceMonitor.registerStatistic(CpuUsrRatio);
    sourceMonitor.registerStatistic(CpuSysRatio);
    sourceMonitor.registerStatistic(CpuGstRatio);
    sourceMonitor.registerStatistic(CpuWaitRatio);
    sourceMonitor.registerStatistic(CpuRatio);
    sourceMonitor.registerStatistic(CpuIndex);

    sourceMonitor.registerPid(pid);
    sourceMonitor.init();
    printf("CpuUsrRatio\tCpuSysRatio\tCpuGstRatio\tCpuWaitRatio\tCpuRatio\tCpuIndex\n");
    for(int i=0;i<count;i++){
        std::this_thread::sleep_for (std::chrono::seconds(interval));
        sourceMonitor.update();
        double cpuUsrRatio=sourceMonitor.getStatistic(pid,CpuUsrRatio);
        double cpuSysRatio=sourceMonitor.getStatistic(pid,CpuSysRatio);
        double cpuGstRatio=sourceMonitor.getStatistic(pid,CpuGstRatio);
        double cpuWaitRatio=sourceMonitor.getStatistic(pid,CpuWaitRatio);
        double cpuRatio=sourceMonitor.getStatistic(pid,CpuRatio);
        double cpuIndex=sourceMonitor.getStatistic(pid,CpuIndex);        
        printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",cpuUsrRatio,cpuSysRatio,cpuGstRatio,cpuWaitRatio,cpuRatio,cpuIndex);
    }    
}

void test4(int count, int interval, int pid){
    SourceMonitor sourceMonitor;
    sourceMonitor.registerStatistic(IoRdRate);
    sourceMonitor.registerStatistic(IoWrRate);
    sourceMonitor.registerStatistic(IoCcwrRate);
    sourceMonitor.registerStatistic(IoDelay);

    sourceMonitor.registerPid(pid);
    sourceMonitor.init();
    printf("IoRdRate\tIoWrRate\tIoCcwrRate\tIoDelay\n");
    for(int i=0;i<count;i++){
        std::this_thread::sleep_for (std::chrono::seconds(interval));
        sourceMonitor.update();
        double ioRdRate=sourceMonitor.getStatistic(pid,IoRdRate);
        double ioWrRate=sourceMonitor.getStatistic(pid,IoWrRate);
        double ioCcwrRate=sourceMonitor.getStatistic(pid,IoCcwrRate);
        double ioDelay=sourceMonitor.getStatistic(pid,IoDelay);      
        printf("%lf\t%lf\t%lf\t%lf\n",ioRdRate,ioWrRate,ioCcwrRate,ioDelay);
    }    
}

void test5(int count, int interval, int pid){
    SourceMonitor sourceMonitor;
    sourceMonitor.registerStatistic(NetSdRate);
    sourceMonitor.registerStatistic(NetRcRate);

    sourceMonitor.registerPid(pid);
    sourceMonitor.init();
    printf("NetSdRate\tNetRcRate\n");
    for(int i=0;i<count;i++){
        std::this_thread::sleep_for (std::chrono::seconds(interval));
        sourceMonitor.update();
        double netSdRate=sourceMonitor.getStatistic(pid,NetSdRate);
        double netRcRate=sourceMonitor.getStatistic(pid,NetRcRate);   
        printf("%lf\t%lf\n",netSdRate,netRcRate);
    }    
}

int main(int argc, char **argv){
    if(argc!=4){
        printf("[usage]: %s count interval pid\n", argv[0]);
        return 0;
    }

    int count=atoi(argv[1]);
    int interval=atoi(argv[2]);
    int pid= atoi(argv[3]);

    test1(count,interval,pid);
    //test2(count,interval,pid);
    //test3(count,interval,pid);
    //test4(count,interval,pid);
    //test5(count,interval,pid);


    return 0;
}
