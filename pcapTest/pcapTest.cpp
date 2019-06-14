#include "decpcap.h"
#include <arpa/inet.h>  //in_addr inet_aton

#include <iostream>

#include <thread>    
#include <chrono>  

#include <netinet/ip.h> //struct ip
#include <netinet/ip6.h> //struct ip6

#include <signal.h> //  signal SIGINT

char localAddrStr1[20]="10.18.133.10";
struct in_addr localAddr1;
char localAddrStr2[20]="10.18.134.10";
struct in_addr localAddr2;


int sentBytes=0;
int rcvBytes=0;

void getLocalAddr(){
  if(!inet_aton(localAddrStr1,&localAddr1)||!inet_aton(localAddrStr2,&localAddr2) )
    printf("get local address error!!!\n");
  printf("local addr1:%s\tlocal addr2:%s",inet_ntoa(localAddr1),inet_ntoa(localAddr2));
}

void quit_cb(int iSignNo/* i */) {
  printf("Capture sign no:%d\n",iSignNo);
  exit(0);
}

bool wait_for_next_trigger() {
  std::this_thread::sleep_for (std::chrono::microseconds(1));
  return true;
}


struct dpargs {
  const char *device;
  int sa_family;
  in_addr ip_src;
  in_addr ip_dst;
  in6_addr ip6_src;
  in6_addr ip6_dst;
};


int process_tcp(u_char *userdata, const dp_header *header,const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  struct tcphdr *tcp = (struct tcphdr *)m_packet;

  /* get info from userdata, then call getPacket */
  switch (args->sa_family) {
  case AF_INET:
    break;
  case AF_INET6:
    break;
  default:
    std::cerr << "Invalid address family for TCP packet: " << args->sa_family << std::endl;
    return true;
  }

  /* we're done now. */
  return true;
}

int process_udp(u_char *userdata, const dp_header *header, const u_char *m_packet) {
  struct dpargs *args = (struct dpargs *)userdata;
  struct udphdr *udp = (struct udphdr *)m_packet;

  switch (args->sa_family) {
  case AF_INET:
    break;
  case AF_INET6:
    break;
  default:
    std::cerr << "Invalid address family for UDP packet: " << args->sa_family
              << std::endl;
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

  if(args->ip_src.s_addr==localAddr1.s_addr||args->ip_src.s_addr==localAddr2.s_addr){
    sentBytes+=header->len;
  }else if(args->ip_dst.s_addr==localAddr1.s_addr||args->ip_dst.s_addr==localAddr2.s_addr){
  // }else{
    rcvBytes+=header->len;
  }
  // char src[100]={0};
  // char dst[100]={0};
  // printf();

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


int main(int argc, char **argv) {

  int promisc = 0;
  char *filter = NULL;
  char errbuf[PCAP_ERRBUF_SIZE];
  char devName[100]="eth1";

  getLocalAddr();

  dp_handle *newhandle =  dp_open_live(devName, BUFSIZ, promisc, 100, filter, errbuf);
  if (newhandle != NULL) {
    dp_addcb(newhandle, dp_packet_ip, process_ip);
    dp_addcb(newhandle, dp_packet_ip6, process_ip6);
    dp_addcb(newhandle, dp_packet_tcp, process_tcp);
    dp_addcb(newhandle, dp_packet_udp, process_udp);

    /* The following code solves sf.net bug 1019381, but is only available
      * in newer versions (from 0.8 it seems) of libpcap
      *
      * update: version 0.7.2, which is in debian stable now, should be ok
      * also.
      */
    if (dp_setnonblock(newhandle, 1, errbuf) == -1) {
      fprintf(stderr, "Error putting libpcap in nonblocking mode\n");
    }
    // handles = new handle(newhandle, current_dev->name, handles);

  } else {
    fprintf(stderr, "Error opening handler for device %s\n",devName);
  }

  signal(SIGINT, &quit_cb);

  struct dpargs *userdata = (dpargs *)malloc(sizeof(struct dpargs));

  int count=0;
  // Main loop:
  while (1) {
    bool packets_read = false;

    userdata->device = devName;
    userdata->sa_family = AF_UNSPEC;
    int retval = dp_dispatch(newhandle, -1, (u_char *)userdata,sizeof(struct dpargs));
    if (retval == -1)
      std::cerr << "Error dispatching for device " << devName <<": " << dp_geterr(newhandle) << std::endl;
    else if (retval < 0)
      std::cerr << "Error dispatching for device " << devName <<": " << retval << std::endl;
    else if (retval != 0)
      packets_read = true;

    if(count%1000==0){
      printf("send----receive:\t%d----%d\n",sentBytes,rcvBytes);
    }
    count++;

    // if not packets, do a select() until next packet
    if (!packets_read)
      if (!wait_for_next_trigger())
        // Shutdown requested - exit the loop
        break;
  }

  // clean_up();
}

