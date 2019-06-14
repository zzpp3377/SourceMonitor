#ifndef NETIOMONITORPCAP_HPP
#define NETIOMONITORPCAP_HPP
#include "netIoMonitorType.hpp"
#include <mutex>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <arpa/inet.h>
#include "decpcap.h"

class NetIoMonitorPcap{
public:
    NetIoMonitorPcap(){;}
    NetIoMonitorPcap(int snaplen,int timeOut_ms):snaplen(snaplen),timeOut_ms(timeOut_ms){}
    void setFilter(std::string filter);
    void setPidNetStatus(std::map<unsigned int,NetStatus> pidNetStatus);
    bool setDevices(std::set<std::string> devices);
    bool setTcp2Pid(std::map<TcpInfo, unsigned int> tcp2Pid);
    bool setUdp2Pid(std::map<UdpInfo, unsigned int> udp2Pid);
    std::map<unsigned int,NetStatus> getNetStatus();
    void netIoMonitorPcapLoop();
    ~NetIoMonitorPcap(){;}

public:
    friend int process_tcp(u_char *userdata, const dp_header *header,const u_char *m_packet);
    friend int process_udp(u_char *userdata, const dp_header *header, const u_char *m_packet);
    friend int process_ip(u_char *userdata, const dp_header * header , const u_char *m_packet);
    friend int process_ip6(u_char *userdata, const dp_header * /* header */,const u_char *m_packet);
protected:
    std::string device;
    int snaplen;  
    int timeOut_ms;
    std::string filter;

    std::mutex mtxTcp2Pid;
    std::map<TcpInfo, unsigned int> tcp2Pid;
    std::mutex mtxUdp2Pid;    
    std::map<UdpInfo, unsigned int> udp2Pid;
    std::mutex mtxStatus;
    std::map<unsigned int,NetStatus> pidNetStatus;
};

struct dpargs {
  const char *device;
  int sa_family;
  struct in_addr ip_src;
  struct in_addr ip_dst;
  struct in6_addr ip6_src;
  struct in6_addr ip6_dst;
  NetIoMonitorPcap * monitorPtr;
};

#endif