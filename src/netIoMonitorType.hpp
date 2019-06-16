/*
 * netIoMonitorType.cpp
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

#ifndef NETIOMONITORTYPE_HPP
#define NETIOMONITORTYPE_HPP

class NetStatus{
public:
    //net
    unsigned long long netSd_bytes=0;
    unsigned long long netRc_bytes=0;
};

class TcpInfo{
public:
    friend bool operator<(const TcpInfo& tcp1, const TcpInfo& tcp2);
    TcpInfo(unsigned int ip,unsigned int port):ip(ip),port(port){};
    TcpInfo(){
        TcpInfo(0,0);
    }
public:
    unsigned int ip; // ipv4 
    unsigned int port;
};
inline bool operator<(const TcpInfo& tcp1, const TcpInfo& tcp2){
    return tcp1.port<tcp2.port;
}

class UdpInfo{
public:
    friend bool operator<(const UdpInfo& udp1, const UdpInfo& udp2);
    UdpInfo(unsigned int ip,unsigned int port):ip(ip),port(port){};
    UdpInfo(){
        UdpInfo(0,0);
    }
public:
    unsigned int ip; // ipv4 
    unsigned int port;
};
inline bool operator<(const UdpInfo& udp1, const UdpInfo& udp2){
    return udp1.port<udp2.port;
}

#endif