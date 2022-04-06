#pragma once
#ifndef _ARP_SPOOF_H
#define _ARP_SPOOF_H 1

#include <arpa/inet.h>      // 定义网络地址操作
#include <netinet/ether.h>  // contain ehter_addr
#include <netinet/in.h>     // contain in_addr
#include <sys/types.h>

/**
 * @brief ARP 报文格式
 * @details
 *   bits:    0                  15                 31 \n
 *            +-------------------------------------+  \n
 *  bytes:  0 |        Hardware type (HTYPE)        |  \n
 *            +-------------------------------------+  \n
 *          2 |        Protocol type (PTYPE)        |  \n
 *            +------------------+------------------+  \n
 *          4 | Hardware address | Protocol address |  \n
 *            |  length (HLEN)   |  length (PLEN)   |  \n
 *            +------------------+------------------+  \n
 *          6 |          Operation (OPER)           |  \n
 *            +-------------------------- ----------+  \n
 *          8 |    Sender hardware address (SHA)    |  \n
 *            |              ---------              |  \n
 *            |              ---------              |  \n
 *            +-------------------------------------+  \n
 *         14 |    Sender protocol address (SPA)    |  \n
 *            |              ---------              |  \n
 *            +-------------------------------------+  \n
 *         18 |    Target hardware address (THA)    |  \n
 *            |              ---------              |  \n
 *            |              ---------              |  \n
 *            +-------------------------------------+  \n
 *         24 |    Target protocol address (TPA)    |  \n
 *            |              ---------              |  \n
 *            +-------------------------------------+  \n
 */
struct arp_frame {
    ether_header eth_hdr; // 以太网头
    uint16_t htype;  // 硬件类型
    uint16_t ptype;  // 协议类型
    uint8_t  hlen;   // 硬件地址长度
    uint8_t  plen;   // 协议地址长度
    uint16_t oper;   // 操作码
    uint8_t  sha[6]; // 源硬件地址
    uint8_t  spa[4]; // 源协议地址
    uint8_t  tha[6]; // 目标硬件地址
    uint8_t  tpa[4]; // 目标协议地址
};

// typedef ether_arp arp2

#define ETH_HLEN 14    // 14B 以太网首部长度
#define ARP_LEN 28     // 28B ARP 报文长度
#define ETH_ARP_LEN 42 // arp 以太网帧长度
#endif
