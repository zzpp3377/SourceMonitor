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