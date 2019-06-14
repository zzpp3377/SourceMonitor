#include "netIoMonitor.hpp"
#include <thread>

#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>

#include <ifaddrs.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>


#include "logger.hpp"
static LogLevel logLevel=LOG_LEVEL_WARN;

bool NetIoMonitor::registerPid(unsigned int pid){
    if(registeredPids.count(pid)!=0)return false;
    registeredPids.emplace(pid);
    return true;
}

bool NetIoMonitor::init(){
    //init infomation from main-thread to net-io-thread
    getIPDeviceInfo();
    getInodeInfo();
    pid2TcpUdp();
    //init information from net-io-thread to main-thread
    std::map<unsigned int,NetStatus> pidNetStatus;
    for(auto& pid: registeredPids){
        pidNetStatus[pid]=NetStatus();
        log_debug(logLevel,"pid: %d",pid);
    }
    //generate filter
    generFilter();
    //start net-io-thread
    netIoMonitorPcap.setFilter(filter);
    netIoMonitorPcap.setPidNetStatus(pidNetStatus);
    netIoMonitorPcap.setDevices(devices);
    netIoMonitorPcap.setTcp2Pid(tcp2Pid);
    netIoMonitorPcap.setUdp2Pid(udp2Pid);   
    std::thread netIoThread(&NetIoMonitorPcap::netIoMonitorPcapLoop,&netIoMonitorPcap);
    netIoThread.detach();
    return true;
}

void NetIoMonitor::update(){
    getIPDeviceInfo();
    getInodeInfo();
    pid2TcpUdp();
    netIoMonitorPcap.setDevices(devices);
    netIoMonitorPcap.setTcp2Pid(tcp2Pid);
    netIoMonitorPcap.setUdp2Pid(udp2Pid); 
}

std::map<unsigned int,NetStatus> NetIoMonitor::getNetStatus(){
    return netIoMonitorPcap.getNetStatus();
}

void NetIoMonitor::getIPDeviceInfo(){
    struct ifaddrs * ifaddr, *ifa;
    struct sockaddr_in * addr;
    if (getifaddrs(&ifaddr) == -1) {
        log_err(logLevel,"getifaddrs error");
        return;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)continue;
        if (ifa->ifa_addr->sa_family!=AF_INET)continue;
        addr=(struct sockaddr_in *)(ifa->ifa_addr);
        if(ipSet.count(addr->sin_addr.s_addr)==0){
            ipSet.emplace(addr->sin_addr.s_addr);
            log_debug(logLevel,"ip2Device--addr: %x dev: %s",addr->sin_addr.s_addr,ifa->ifa_name);
        }
        if(ip2Device.count(addr->sin_addr.s_addr)==0){
            ip2Device[addr->sin_addr.s_addr]=ifa->ifa_name;
        }
    }
    freeifaddrs(ifaddr);
}

// /proc/net/tcp /proc/net/udp
void NetIoMonitor::getInodeInfo(){
    unsigned int ip;
    unsigned int port;
    unsigned int inode;
    char tempC;
    unsigned int tempInt;
    
    std::fstream fs;
    char line[256]={0};  //magic number 256, line max length
    
    fs.open(FL_TCPINFO,std::fstream::in);
    fs.getline(line,256);
    while(fs.getline(line,256)){
        unsigned int ip=0;
        unsigned int port=0;
        unsigned int inode=0;
        std::stringstream stst;
        stst.str(std::string(line));
        stst.ignore(256,':');
        stst>>std::hex>>ip>>std::dec;
        stst>>tempC;
        stst>>std::hex>>port>>std::dec;
        stst.ignore(256,':');
        stst.ignore(256,':');
        stst.ignore(256,':');
        stst>>std::hex>>tempInt>>std::dec;
        log_debug(logLevel,"tempInt1: %x",tempInt);
        stst>>std::hex>>tempInt>>std::dec;
        log_debug(logLevel,"tempInt2: %x",tempInt);
        stst>>tempInt;
        log_debug(logLevel,"tempInt3: %x",tempInt);
        stst>>tempInt;
        log_debug(logLevel,"tempInt4: %x",tempInt);
        stst>>inode;
        log_debug(logLevel,"inode2Tcp--read--inode: %d ip: %x port: %d",inode,ip,port);
        if(inode2Tcp.count(inode)==0)inode2Tcp[inode]=TcpInfo(ip,port);
    }
    fs.close();

    fs.open(FL_UDPINFO,std::fstream::in);
    fs.getline(line,256);
    while(fs.getline(line,256)){
        std::stringstream stst;
        stst.str(std::string(line));
        stst.ignore(256,':');
        stst>>std::hex>>ip>>std::dec;
        stst>>tempC;
        stst>>std::hex>>port>>std::dec;
        stst.ignore(256,':');
        stst.ignore(256,':');
        stst.ignore(256,':');
        stst>>std::hex>>tempInt>>std::dec;
        stst>>std::hex>>tempInt>>std::dec;
        stst>>tempInt;
        stst>>tempInt;
        stst>>inode;
        if(inode2Udp.count(inode)==0)inode2Udp[inode]=UdpInfo(ip,port);
    }
    fs.close();
    for(auto ite:inode2Tcp){
        log_debug(logLevel,"inode2Tcp--inode: %d ip: %x port: %d",ite.first,ite.second.ip,ite.second.port);
    }
    for(auto ite:inode2Udp){
        log_debug(logLevel,"inode2Udp--inode: %d ip: %x port: %d",ite.first,ite.second.ip,ite.second.port);
    }
}


// /proc/pid/fd
void NetIoMonitor::pid2TcpUdp(){
    DIR *dp;
	struct dirent *entry;
    char link[256]={};//magic number 256, max path name
    int linkLen; 
    unsigned int inode;
    
    for(auto pid: registeredPids){
        log_debug(logLevel,"pid: %d",pid);
    }
    
    for(auto pid: registeredPids){
        char dirName[100]={0};
        sprintf(dirName,FL_PID_FD,pid);
        if((dp=opendir(dirName))==NULL){
            log_err(logLevel,"Can`t open directory %s",dirName);
            return;
        }
        std::string path(dirName);
        while((entry=readdir(dp))!=NULL){
            std::string file(entry->d_name);
            file=path+file;
            if((linkLen=readlink(file.c_str(),link,256))<0){
                log_info(logLevel,"entry->d_name: %s linkLen: %d",file.c_str(),linkLen);
                continue;
            }
            std::string linkStr(link,linkLen);

            log_debug(logLevel,"linkStr: %s",linkStr.c_str());
            
            if(linkStr.find("socket")==std::string::npos) continue; //magic string socket
            sscanf(linkStr.c_str(),"socket:[%u]",&inode);

            log_debug(logLevel,"inode: %d",inode);
            
            if(inode2Tcp.count(inode)!=0){
                if(inode2Tcp[inode].ip!=0){
                    tcp2Pid[inode2Tcp[inode]]=pid;
                    devices.emplace(ip2Device[inode2Tcp[inode].ip]);
                }else{
                    for(auto &ip: ipSet){
                        TcpInfo tempTcp(ip,inode2Tcp[inode].port);
                        tcp2Pid[tempTcp]=pid;
                        devices.emplace(ip2Device[ip]);
                    }
                }                
            }else if(inode2Udp.count(inode)!=0){
                if(inode2Udp[inode].ip!=0){
                    udp2Pid[inode2Udp[inode]]=pid;
                    devices.emplace(ip2Device[inode2Udp[inode].ip]);
                }else{
                    for(auto &ip: ipSet){
                        UdpInfo tempUdp(ip,inode2Udp[inode].port);
                        udp2Pid[tempUdp]=pid;
                        devices.emplace(ip2Device[ip]);
                    }
                }
            }
        }
    }
}