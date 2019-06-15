/*
 * netIoMonitor.hpp
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