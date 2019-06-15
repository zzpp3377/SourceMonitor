/*
 * monitorImpl.hpp
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

#ifndef MONITOR_IMPL_HPP
#define MONITOR_IMPL_HPP

#include "sourceMonitor.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>

#include "netIoMonitor.hpp"

//read file flag
#define RD_PID_STAT_FLG         0x01
#define RD_PID_SCHEDSTAT_FLG    0x02
#define RD_UPTIME_FLG           0x04
#define RD_MEMINFO_FLG          0x08
#define RD_PID_IO_FLG           0x10 
#define RD_NET_IO_FLG           0x20




//file name 
#define FL_PID_STAT	    "/proc/%u/stat"
#define FL_PID_IO	    "/proc/%u/io"
#define FL_PID_SCHED	"/proc/%u/schedstat"
#define FL_MEMINFO	    "/proc/meminfo"
#define FL_UPTIME       "/proc/uptime"


/* Number of bit shifts to convert pages to kB */
extern unsigned int kb_shift;
#define PG_TO_KB(k)	((k) << kb_shift)

#define B_TO_KB(k)	( (k) / 1024.0 )

class Status{
public:
    //cpu
    unsigned long long utime=0;
    unsigned long long stime=0;
    unsigned long long gtime=0;
    unsigned long long wtime=0;
    unsigned int       processor=0;
    unsigned long long uptime=0;

    //mem
    unsigned long long minflt=0;
    unsigned long long majflt=0;
    unsigned long long vsz=0;
    unsigned long long rss=0;
    unsigned long long tlmkb=0;

    //io
    unsigned long long read_bytes=0;
    unsigned long long write_bytes=0;
    unsigned long long cancelled_write_bytes=0;
    unsigned long long blkio_swapin_delays=0;

    //net
    unsigned long long netSd_bytes=0;
    unsigned long long netRc_bytes=0;

};

class MonitorImpl{
public:
    MonitorImpl(){};
    bool registerStatistic(StatisticT statistic);
    bool registerPid(unsigned int pid);
    bool init();
    void update();
    double getStatistic(unsigned int pid,StatisticT statistic);
    ~MonitorImpl(){};
private:
    void readProcPidStat(); //cpu and mem and io:blkio_swapin_delays
    void readProcPidSchedstat(); //cpu:processor
    void readProcUptime(); //cpu:uptime
    void readProcMeminfo(); //mem:tlmkb
    void readProcPidIo(); //io
    void readNetIo();//net io
public:
    //cpu
    double getCpuRatio(unsigned int pid);
    double getCpuUsrRatio(unsigned int pid);
    double getCpuSysRatio(unsigned int pid);
    double getCpuGstRatio(unsigned int pid);
    double getCpuWaitRatio(unsigned int pid);
    double getCpuIndex(unsigned int pid);
    //mem
    double getMemRatio(unsigned int pid);
    double getMemRss(unsigned int pid);
    double getMemVsz(unsigned int pid);
    double getMemMinfltRate(unsigned int pid);
    double getMemMajfltRate(unsigned int pid);
    //io
    double getIoRdRate(unsigned int pid);
    double getIoWrRate(unsigned int pid);
    double getIoCcwrRate(unsigned int pid);
    double getIoDelay(unsigned int pid);
    //net io
    double getNetSdRate(unsigned int pid);
    double getNetRcRate(unsigned int pid);

private:
    int cur=0;
    unsigned int rdFlg=0;
    std::map<StatisticT,std::function<double(MonitorImpl* const,unsigned int)>> handlers;
    std::map<unsigned int,std::vector<Status>> pidStatus;
    std::set<unsigned int> registeredPids;
    NetIoMonitor netIoMonitor;
};





#endif