/*
 * netIoMonitorPcap.cpp
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
#include "netIoMonitorType.hpp"
#include "netIoMonitorPcap.hpp"
#include <netinet/in.h>
#include <cstring>

#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#include "logger.hpp"

// static LogLevel logLevel=LOG_LEVEL_DEBUG;
static LogLevel logLevel=LOG_LEVEL_WARN;

int process_tcp(u_char *userdata, const dp_header *header,const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  struct tcphdr *tcp = (struct tcphdr *)m_packet;
  NetIoMonitorPcap * monitorPtr = args->monitorPtr;

  /* get info from userdata, then call getPacket */
  switch (args->sa_family) {
  case AF_INET:{
      TcpInfo tcpInfoSrc(args->ip_src.s_addr,ntohs(tcp->th_sport));
      TcpInfo tcpInfoDst(args->ip_dst.s_addr,ntohs(tcp->th_dport));
      log_debug(logLevel,"tcpInfoSrc ip: %x port: %d",args->ip_src.s_addr,ntohs(tcp->th_sport));
      log_debug(logLevel,"tcpInfoDst ip: %x port: %d",args->ip_dst.s_addr,ntohs(tcp->th_dport));
      int judge=0;   //0--don't match,1--send,2--receive 
      monitorPtr->mtxTcp2Pid.lock();
      if(monitorPtr->tcp2Pid.count(tcpInfoSrc)!=0){
          judge=1;
      }else if(monitorPtr->tcp2Pid.count(tcpInfoDst)!=0){
          judge=2;
      }
      monitorPtr->mtxTcp2Pid.unlock();
      monitorPtr->mtxStatus.lock();
      if(judge==1){
          monitorPtr->pidNetStatus[monitorPtr->tcp2Pid[tcpInfoSrc]].netSd_bytes+= header->len;
          log_debug(logLevel,"pidNetStatus[%d].netSd_bytes: %llu",monitorPtr->tcp2Pid[tcpInfoSrc],monitorPtr->pidNetStatus[monitorPtr->tcp2Pid[tcpInfoSrc]].netSd_bytes);
      }else if(judge==2){
          monitorPtr->pidNetStatus[monitorPtr->tcp2Pid[tcpInfoDst]].netRc_bytes+= header->len;
          log_debug(logLevel,"pidNetStatus[%d].netRc_bytes: %llu",monitorPtr->tcp2Pid[tcpInfoDst],monitorPtr->pidNetStatus[monitorPtr->tcp2Pid[tcpInfoDst]].netRc_bytes);
      }
      monitorPtr->mtxStatus.unlock();
      break;
  }
  case AF_INET6:
    break;
  default:
    log_err(logLevel,"Invalid address family for TCP packet: %d",args->sa_family);
    return true;
  }

  /* we're done now. */
  return true;
}

int process_udp(u_char *userdata, const dp_header *header, const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  struct udphdr *udp = (struct udphdr *)m_packet;
  NetIoMonitorPcap * monitorPtr = args->monitorPtr;
  log_debug(logLevel,"into process_udp");

  switch (args->sa_family) {
  case AF_INET:{
      UdpInfo udpInfoSrc(args->ip_src.s_addr,ntohs(udp->uh_sport));
      UdpInfo udpInfoDst(args->ip_dst.s_addr,ntohs(udp->uh_dport));
      log_debug(logLevel,"udpInfoSrc ip: %x port: %d",args->ip_src.s_addr,ntohs(udp->uh_sport));
      log_debug(logLevel,"udpInfoDst ip: %x port: %d",args->ip_dst.s_addr,ntohs(udp->uh_dport));
      int judge=0;   //0--don't match,1--send,2--receive 
      monitorPtr->mtxUdp2Pid.lock();
      if(monitorPtr->udp2Pid.count(udpInfoSrc)!=0){
          judge=1;
      }else if(monitorPtr->udp2Pid.count(udpInfoDst)!=0){
          judge=2;
      }
      monitorPtr->mtxUdp2Pid.unlock();
      monitorPtr->mtxStatus.lock();
      if(judge==1){
          monitorPtr->pidNetStatus[monitorPtr->udp2Pid[udpInfoSrc]].netSd_bytes+= header->len;
          log_debug(logLevel,"pidNetStatus[%d].netSd_bytes: %llu",monitorPtr->udp2Pid[udpInfoSrc],monitorPtr->pidNetStatus[monitorPtr->udp2Pid[udpInfoSrc]].netSd_bytes);
      }else if(judge==2){
          monitorPtr->pidNetStatus[monitorPtr->udp2Pid[udpInfoDst]].netRc_bytes+= header->len;
          log_debug(logLevel,"pidNetStatus[%d].netRc_bytes: %llu",monitorPtr->udp2Pid[udpInfoDst],monitorPtr->pidNetStatus[monitorPtr->udp2Pid[udpInfoDst]].netRc_bytes);
      }
      monitorPtr->mtxStatus.unlock();
      break;
  }
  case AF_INET6:
    break;
  default:
    log_err(logLevel,"Invalid address family for UDP packet: %d",args->sa_family);          
    return true;
  }

  /* we're done now. */
  return true;
}

int process_ip(u_char *userdata, const dp_header * header , const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  struct ip *ip = (struct ip *)m_packet;
  args->sa_family = AF_INET;
  args->ip_src = ip->ip_src;
  args->ip_dst = ip->ip_dst;

  // if(args->ip_src.s_addr==localAddr1.s_addr||args->ip_src.s_addr==localAddr2.s_addr){
  //   sentBytes+=header->len;
  // }else if(args->ip_dst.s_addr==localAddr1.s_addr||args->ip_dst.s_addr==localAddr2.s_addr){
  //   rcvBytes+=header->len;
  // }

  /* we're not done yet - also parse tcp :) */
  return false;
}

int process_ip6(u_char *userdata, const dp_header * /* header */,const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  const struct ip6_hdr *ip6 = (struct ip6_hdr *)m_packet;
  args->sa_family = AF_INET6;
  args->ip6_src = ip6->ip6_src;
  args->ip6_dst = ip6->ip6_dst;

  /* we're not done yet - also parse tcp :) */
  return false;
}

bool NetIoMonitorPcap::setDevices(std::set<std::string> devices){
    // if(devices.size()==0)return false;
    // if(devices.size()==1)device=*(devices.begin());
    // if(devices.size()>1)device="any";    //magic string according to libpcap
    device="any";
    return true;
}

bool NetIoMonitorPcap::setTcp2Pid(std::map<TcpInfo, unsigned int> tcp2Pid){
    mtxTcp2Pid.lock();
    this->tcp2Pid=tcp2Pid;
    mtxTcp2Pid.unlock();
    log_debug(logLevel,"tcp2Pid size: %lu",tcp2Pid.size());
    for(auto ite: tcp2Pid){
        log_debug(logLevel,"tcp--ip: %x port: %d pid: %d",ite.first.ip,ite.first.port,ite.second);
    }
    return true;
}

bool NetIoMonitorPcap::setUdp2Pid(std::map<UdpInfo, unsigned int> udp2Pid){
    mtxUdp2Pid.lock();
    this->udp2Pid=udp2Pid;
    mtxUdp2Pid.unlock();
    log_debug(logLevel,"udp2Pid size: %lu",udp2Pid.size());
    for(auto ite: udp2Pid){
        log_debug(logLevel,"udp--ip: %x port: %d pid: %d",ite.first.ip,ite.first.port,ite.second);
    }
    return true;
}

void NetIoMonitorPcap::setFilter(std::string filter){
    this->filter=filter;
}

void NetIoMonitorPcap::setPidNetStatus(std::map<unsigned int,NetStatus> pidNetStatus){
    this->pidNetStatus=pidNetStatus;
    for(auto ite: pidNetStatus){
        log_debug(logLevel,"pidstatus--pid: %d",ite.first);
    }
}

std::map<unsigned int,NetStatus> NetIoMonitorPcap::getNetStatus(){
    mtxStatus.lock();
    std::map<unsigned int,NetStatus> result=pidNetStatus;
    mtxStatus.unlock();
    for(auto ite: pidNetStatus){
        log_debug(logLevel,"pidstatus--pid: %d netSd_bytes: %llu netRc_bytes: %llu",ite.first,ite.second.netSd_bytes,ite.second.netRc_bytes);
    }
    return result;
}

void NetIoMonitorPcap::netIoMonitorPcapLoop(){
    char errbuf[PCAP_ERRBUF_SIZE];
    char * filterC = new char [filter.length()+1];
    std::strcpy (filterC, filter.c_str());
    dp_handle *newhandle =  dp_open_live(device.c_str(), snaplen, 0, timeOut_ms, filterC, errbuf); // magic number 0--not promisc,
    delete filterC;
    if (newhandle != NULL) {
        dp_addcb(newhandle, dp_packet_ip, process_ip);
        dp_addcb(newhandle, dp_packet_ip6, process_ip6);
        dp_addcb(newhandle, dp_packet_tcp, process_tcp);
        dp_addcb(newhandle, dp_packet_udp, process_udp);
    } else {
        log_err(logLevel,"Error opening handler for device %s",device.c_str());
        return;
    }
    struct dpargs *userdata = (dpargs *)malloc(sizeof(struct dpargs));
    while(1){
        char * deviceC=new char [device.length()+1];
        std::strcpy (deviceC, device.c_str());
        userdata->device = deviceC;
        userdata->sa_family = AF_UNSPEC;
        userdata->monitorPtr = this;
        int retval = dp_dispatch(newhandle, -1, (u_char *)userdata,sizeof(struct dpargs));// magic number -1--cnt,according to libpcap
        delete userdata->device;
        if(retval<0){
            log_err(logLevel,"Error dispatching for device %s : %d",device.c_str(),retval);
            return ;
        }
    }
}



#endif