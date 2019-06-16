/*
 * sourceMonitor.hpp
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
#ifndef SOURCE_MONITOR_HPP
#define SOURCE_MONITOR_HPP

typedef enum StatisticType{
    CpuRatio=0,
    CpuUsrRatio,
    CpuSysRatio,
    CpuGstRatio,
    CpuWaitRatio,
    CpuIndex,

    MemRatio=1000,
    MemRss,
    MemVsz,
    MemMinfltRate,
    MemMajfltRate,

    IoRdRate=2000,
    IoWrRate,
    IoCcwrRate,
    IoDelay,

    NetSdRate=3000,
    NetRcRate
} StatisticT;



class MonitorImpl;
class SourceMonitor{
public:
    SourceMonitor();
    bool registerStatistic(StatisticT statistic);
    bool registerPid(unsigned int pid);
    bool init();
    void update();
    double getStatistic(unsigned int pid,StatisticT statistic);
    ~SourceMonitor();
private:
    MonitorImpl * monitorImpl;
};

#endif