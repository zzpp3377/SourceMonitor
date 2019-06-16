/*
 * monitorImpl.cpp
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



#include "monitorImpl.hpp"
#include <string>
#include <fstream> 
#include <stdio.h>

#include <unistd.h>

#include "logger/logger.hpp"

unsigned int kb_shift;

static LogLevel logLevel=LOG_LEVEL_WARN;

//map from statistic to file flag
std::map<StatisticT,unsigned int> sttt2file={
    //cpu
    {CpuRatio, (RD_UPTIME_FLG|RD_PID_STAT_FLG)},
    {CpuUsrRatio, (RD_UPTIME_FLG|RD_PID_STAT_FLG)},
    {CpuSysRatio, (RD_UPTIME_FLG|RD_PID_STAT_FLG)},
    {CpuGstRatio, (RD_UPTIME_FLG|RD_PID_STAT_FLG)},
    {CpuWaitRatio, (RD_UPTIME_FLG|RD_PID_SCHEDSTAT_FLG)},
    {CpuIndex, RD_PID_STAT_FLG},
    //mem
    {MemRatio, (RD_PID_STAT_FLG|RD_MEMINFO_FLG)},
    {MemRss, RD_PID_STAT_FLG},
    {MemVsz, RD_PID_STAT_FLG},
    {MemMinfltRate, (RD_PID_STAT_FLG|RD_UPTIME_FLG)},
    {MemMajfltRate, (RD_PID_STAT_FLG|RD_UPTIME_FLG)},
    //io
    {IoRdRate, (RD_PID_IO_FLG|RD_UPTIME_FLG)},
    {IoWrRate, (RD_PID_IO_FLG|RD_UPTIME_FLG)},
    {IoCcwrRate, (RD_PID_IO_FLG|RD_UPTIME_FLG)},
    {IoDelay, RD_PID_STAT_FLG},
    //net
    {NetSdRate, (RD_NET_IO_FLG|RD_UPTIME_FLG)},
    {NetRcRate, (RD_NET_IO_FLG|RD_UPTIME_FLG)}
};

//map from statistic to method
std::map<StatisticT,std::function<double(MonitorImpl* const,unsigned int)>> sttt2method={
    //cpu
    {CpuRatio, &MonitorImpl::getCpuRatio},
    {CpuUsrRatio, &MonitorImpl::getCpuUsrRatio},
    {CpuSysRatio, &MonitorImpl::getCpuSysRatio},
    {CpuGstRatio, &MonitorImpl::getCpuGstRatio},
    {CpuWaitRatio, &MonitorImpl::getCpuWaitRatio},
    {CpuIndex, &MonitorImpl::getCpuIndex},
    //mem
    {MemRatio, &MonitorImpl::getMemRatio},
    {MemRss, &MonitorImpl::getMemRss},
    {MemVsz, &MonitorImpl::getMemVsz},
    {MemMinfltRate, &MonitorImpl::getMemMinfltRate},
    {MemMajfltRate, &MonitorImpl::getMemMajfltRate},
    //io
    {IoRdRate, &MonitorImpl::getIoRdRate},
    {IoWrRate, &MonitorImpl::getIoWrRate},
    {IoCcwrRate, &MonitorImpl::getIoCcwrRate},
    {IoDelay, &MonitorImpl::getIoDelay},
    //net
    {NetSdRate, &MonitorImpl::getNetSdRate},
    {NetRcRate, &MonitorImpl::getNetRcRate}
};


bool MonitorImpl::registerStatistic(StatisticT statistic){
    if(!sttt2file.count(statistic))return false;  //add log
    rdFlg=rdFlg|sttt2file[statistic];
    handlers[statistic]=sttt2method[statistic];
    return true;
}

bool MonitorImpl::registerPid(unsigned int pid){
    if(registeredPids.count(pid))return false;
    log_debug(logLevel,"pid: %u",pid);
    pidStatus[pid]=std::vector<Status>(2);
    registeredPids.emplace(pid);
    netIoMonitor.registerPid(pid);
    return true;
}

bool MonitorImpl::init(){
    //init kb_shift
    int size=0;
    int shift=0;
    if ((size = sysconf(_SC_PAGESIZE)) == -1) return false;
    size >>= 10;	/* Assume that a page has a minimum size of 1 kB */
	while (size > 1) {
		shift++;
		size >>= 1;
	}
	kb_shift = (unsigned int) shift;

    // init the first pidStatus[pid][cur]
    if(rdFlg&RD_MEMINFO_FLG){
        readProcMeminfo();
    }
    //init the netIoMonitor
    if(rdFlg&RD_NET_IO_FLG)netIoMonitor.init();
    update();
}

void MonitorImpl::update(){
    if(rdFlg&RD_PID_STAT_FLG){
        readProcPidStat();
    }
    if(rdFlg&RD_PID_SCHEDSTAT_FLG){
        readProcPidSchedstat();
    }
    if(rdFlg&RD_UPTIME_FLG){
        readProcUptime();
    }
    if(rdFlg&RD_PID_IO_FLG){
        readProcPidIo();
    }
    if(rdFlg&RD_NET_IO_FLG){
        readNetIo();
        netIoMonitor.update();
    }
    cur=!cur;
}

double MonitorImpl::getStatistic(unsigned int pid,StatisticT statistic){
    if(!sttt2method.count(statistic)) return -2;
    return handlers[statistic](this,pid);
}

void MonitorImpl::readProcMeminfo(){
    unsigned long long totalMem;
    std::fstream fs;
    fs.open(FL_MEMINFO,std::fstream::in);
    fs.ignore(10,':');
    fs>>totalMem;
    fs.close();
    for(auto &pid: registeredPids){
        for(auto &status: pidStatus[pid]){
            status.tlmkb=totalMem;
        }
    }
} 
void MonitorImpl::readProcPidStat(){
    for(auto &pid: registeredPids){
        char fileName[100]={0};
        sprintf(fileName,FL_PID_STAT,pid);
        std::fstream fs;
        fs.open(fileName,std::fstream::in);
        fs.ignore(1024,')');
        
        char temp1;
        unsigned long long int temp2;
        fs>>temp1;
        for(int i=4;i<10;i++){
            fs>>temp2;
        }
        fs>>pidStatus[pid][cur].minflt;
        fs>>temp2;
        fs>>pidStatus[pid][cur].majflt;
        fs>>temp2;
        fs>>pidStatus[pid][cur].utime;
        fs>>pidStatus[pid][cur].stime;
        for(int i=16;i<23;i++){
            fs>>temp2;
        }
        fs>>pidStatus[pid][cur].vsz;
        pidStatus[pid][cur].vsz=pidStatus[pid][cur].vsz;   
        fs>>pidStatus[pid][cur].rss;
        pidStatus[pid][cur].rss=pidStatus[pid][cur].rss;
        for(int i=25;i<39;i++){
            fs>>temp2;
        }
        printf("\n");
        fs>>pidStatus[pid][cur].processor;
        for(int i=40;i<42;i++){
            fs>>temp2;
        }
        fs>>pidStatus[pid][cur].blkio_swapin_delays;
        fs>>pidStatus[pid][cur].gtime;
        fs.close();
    }
} 

void MonitorImpl::readProcPidSchedstat(){
    for(auto &pid: registeredPids){
        char fileName[100]={0};
        sprintf(fileName,FL_PID_SCHED,pid);
        std::fstream fs;
        fs.open(fileName,std::fstream::in);
        long int temp;
        fs>>temp;
        fs>>pidStatus[pid][cur].wtime;
        fs.close();
    }
} 
void MonitorImpl::readProcUptime(){
    std::fstream fs;
    fs.open(FL_UPTIME,std::fstream::in);
    double uptime;
    fs>>uptime;
    fs.close();
    for(auto &pid: registeredPids){
        pidStatus[pid][cur].uptime=(unsigned long long)(uptime*100); //magic number 100 from sysconf(_SC_CLK_TCK)
    }
} 

void MonitorImpl::readProcPidIo(){
    for(auto &pid: registeredPids){
        char fileName[100]={0};
        sprintf(fileName,FL_PID_IO,pid);
        std::fstream fs;
        fs.open(fileName,std::fstream::in);
        std::string str;
        while(fs>>str){
            if(str=="read_bytes:")fs>>pidStatus[pid][cur].read_bytes;
            if(str=="write_bytes:")fs>>pidStatus[pid][cur].write_bytes;
            if(str=="cancelled_write_bytes:")fs>>pidStatus[pid][cur].cancelled_write_bytes;
        }        
        fs.close();
    }
}

void MonitorImpl::readNetIo(){
    std::map<unsigned int,NetStatus> netStatus=netIoMonitor.getNetStatus();
    for(auto &pid: registeredPids){
        pidStatus[pid][cur].netSd_bytes=netStatus[pid].netSd_bytes;
        pidStatus[pid][cur].netRc_bytes=netStatus[pid].netRc_bytes;
    }
}

double MonitorImpl::getCpuRatio(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cpuTime=static_cast<double>((pidStatus[pid][cur_last].utime+pidStatus[pid][cur_last].stime)-(pidStatus[pid][pre_last].utime+pidStatus[pid][pre_last].stime));
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cpuTime/wallTime;
}
double MonitorImpl::getCpuUsrRatio(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cpuTime=static_cast<double>(pidStatus[pid][cur_last].utime-pidStatus[pid][pre_last].utime);
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cpuTime/wallTime;
}
double MonitorImpl::getCpuSysRatio(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cpuTime=static_cast<double>(pidStatus[pid][cur_last].stime-pidStatus[pid][pre_last].stime);
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cpuTime/wallTime;
}
double MonitorImpl::getCpuGstRatio(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cpuTime=static_cast<double>(pidStatus[pid][cur_last].gtime-pidStatus[pid][pre_last].gtime);
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cpuTime/wallTime;
}
double MonitorImpl::getCpuWaitRatio(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cpuTime=static_cast<double>(pidStatus[pid][cur_last].wtime-pidStatus[pid][pre_last].wtime);
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cpuTime/wallTime; 
}
double MonitorImpl::getCpuIndex(unsigned int pid){
    int cur_last=!cur;
    return pidStatus[pid][cur_last].processor;
}

double MonitorImpl::getMemRatio(unsigned int pid){
    int cur_last=!cur;
    double rss=static_cast<double>(PG_TO_KB(pidStatus[pid][cur_last].rss));
    double totalMem=static_cast<double>(pidStatus[pid][cur_last].tlmkb);
    return rss/totalMem; 
}
double MonitorImpl::getMemRss(unsigned int pid){
    int cur_last=!cur;
    return static_cast<double>(PG_TO_KB(pidStatus[pid][cur_last].rss));
}
double MonitorImpl::getMemVsz(unsigned int pid){
    int cur_last=!cur;
    return static_cast<double>(B_TO_KB(pidStatus[pid][cur_last].vsz));    
}
double MonitorImpl::getMemMinfltRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double minflt=static_cast<double>(pidStatus[pid][cur_last].minflt-pidStatus[pid][pre_last].minflt)*100; //magic number 100 from sysconf(_SC_CLK_TCK)
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return minflt/wallTime; 
}
double MonitorImpl::getMemMajfltRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double majflt=static_cast<double>(pidStatus[pid][cur_last].majflt-pidStatus[pid][pre_last].majflt)*100; //magic number 100 from sysconf(_SC_CLK_TCK)
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return majflt/wallTime; 
}

double MonitorImpl::getIoRdRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double read_bytes=static_cast<double>(B_TO_KB(pidStatus[pid][cur_last].read_bytes-pidStatus[pid][pre_last].read_bytes))*100; //magic number 100 from sysconf(_SC_CLK_TCK)
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return read_bytes/wallTime; 
}
double MonitorImpl::getIoWrRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double write_bytes=static_cast<double>(B_TO_KB(pidStatus[pid][cur_last].write_bytes-pidStatus[pid][pre_last].write_bytes))*100; //magic number 100 from sysconf(_SC_CLK_TCK)
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return write_bytes/wallTime; 
}
double MonitorImpl::getIoCcwrRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double cancelled_write_bytes=static_cast<double>(B_TO_KB(pidStatus[pid][cur_last].cancelled_write_bytes-pidStatus[pid][pre_last].cancelled_write_bytes))*100; //magic number 100 from sysconf(_SC_CLK_TCK)
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime);
    return cancelled_write_bytes/wallTime; 
}
double MonitorImpl::getIoDelay(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    return static_cast<double>(pidStatus[pid][cur_last].blkio_swapin_delays-pidStatus[pid][pre_last].blkio_swapin_delays)/100.0; //magic number 100 from sysconf(_SC_CLK_TCK)
}

double MonitorImpl::getNetSdRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double netSdByte=static_cast<double>(pidStatus[pid][cur_last].netSd_bytes-pidStatus[pid][pre_last].netSd_bytes)/1000;
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime)/100;
    return netSdByte/wallTime;
}
double MonitorImpl::getNetRcRate(unsigned int pid){
    int cur_last=!cur;
    int pre_last=cur;
    double netRcByte=static_cast<double>(pidStatus[pid][cur_last].netRc_bytes-pidStatus[pid][pre_last].netRc_bytes)/1000; //kB
    double wallTime=static_cast<double>(pidStatus[pid][cur_last].uptime-pidStatus[pid][pre_last].uptime)/100;  // s
    return netRcByte/wallTime;
}