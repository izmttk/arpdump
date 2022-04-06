#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_ 1

#include "arpdump.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <functional>

#include <arpa/inet.h>      // 定义网络地址操作
#include <netinet/ether.h>  // contain ehter_addr
#include <netinet/in.h>     // contain in_addr

#include <sys/ioctl.h>
#include <net/if.h>
#include <netpacket/packet.h>

arp_frame make_arp_request(ether_addr *, in_addr *, ether_addr *, in_addr *);

arp_frame make_arp_reply(ether_addr *, in_addr *, ether_addr *, in_addr *);

sockaddr_ll get_addr_by_name(char* );

void dump_arp(arp_frame *);

void send_arp_to(const int fd, const char* if_name, arp_frame* arp_send);

void recv_arp_from(const int fd, const char* if_name, std::function<void(arp_frame *, sockaddr_ll *)>callback);

#endif