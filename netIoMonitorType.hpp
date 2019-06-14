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