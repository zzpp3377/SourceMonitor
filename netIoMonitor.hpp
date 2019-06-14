#ifndef NETIOMONITOR_HPP 
#define NETIOMONITOR_HPP
 

#include <string>
#include <map>
#include <set>

#include "netIoMonitorType.hpp"
#include "netIoMonitorPcap.hpp"

#define FL_TCPINFO "/proc/net/tcp"
#define FL_UDPINFO "/proc/net/udp"
#define FL_PID_FD   "/proc/%u/fd/"



class NetIoMonitor{
public:
    NetIoMonitor():netIoMonitorPcap(80,1){;}  //magic number  snapLen=80,timeout_ms=1
    bool registerPid(unsigned int pid);  
    bool init(); //init new thread
    void update();
    std::map<unsigned int,NetStatus> getNetStatus();
    ~NetIoMonitor(){;}
protected:    
    void getIPDeviceInfo(); 
    void getInodeInfo();
    void pid2TcpUdp();
    void generFilter(){};//todo
protected:    

    std::map<TcpInfo, unsigned int> tcp2Pid;
    std::map<UdpInfo, unsigned int> udp2Pid;
    std::set<std::string> devices;

    std::set<unsigned int> ipSet;
    std::map<unsigned int, std::string> ip2Device;
    std::map<unsigned int, TcpInfo> inode2Tcp;
    std::map<unsigned int, UdpInfo> inode2Udp;

    
    std::string filter;
    
    std::set<unsigned int> registeredPids;

    NetIoMonitorPcap netIoMonitorPcap;
};



#endif