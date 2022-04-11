#pragma once
#ifndef _FUNDAMENTAL_H
#define _FUNDAMENTAL_H 1

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>

#include <sys/socket.h>

#include <arpa/inet.h>      // 定义网络地址操作
#include <netinet/ether.h>  // contain ehter_addr
#include <netinet/in.h>     // contain in_addr

#include <sys/ioctl.h>
#include <net/if.h>
#include <netpacket/packet.h>

#include <ifaddrs.h>
#include <stdexcept>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cstdio>

#define MAC_LEN 6
#define IP_LEN 4

#define ETH_HLEN 14    // 14B 以太网首部长度
#define ARP_LEN 28     // 28B ARP 报文长度
#define ETH_ARP_LEN 42 // arp 以太网帧长度

class MacAddr {
public:
    std::uint8_t addr[MAC_LEN];
    MacAddr();
    MacAddr(const MacAddr &mac);
    MacAddr(const uint8_t* new_addr);
    MacAddr(const ether_addr& eth);
    MacAddr(const sockaddr_ll& sll);
    MacAddr(const std::string& mac_str);
    std::string to_string() const;
    bool operator == (const MacAddr& mac) const;
    operator ether_addr() const;
};

class IpAddr {
public:
    std::uint8_t addr[IP_LEN];
    IpAddr();
    IpAddr(const IpAddr& ip);
    IpAddr(const uint32_t new_addr);
    IpAddr(const uint8_t* new_addr);
    IpAddr(const in_addr& in);
    IpAddr(const sockaddr_in& sin);
    IpAddr(const std::string& ip_str);
    std::string to_string() const;
    bool operator == (const IpAddr& ip) const;
    operator in_addr() const;
};


// uncompleted network interface class
class Interface {
public:
    std::string name;
    MacAddr mac_addr;
    IpAddr ip_addr;
    IpAddr boardcast_addr;
    IpAddr netmask;
    short int flags;
    // forbid default constructor
    Interface() = delete;
    Interface(const std::string& name, const MacAddr& mac_addr, const IpAddr& ip_addr, const IpAddr& boardcast_addr, const IpAddr& netmask);
    Interface(const Interface& interface);
    Interface& operator=(const Interface& interface);

    // constructor from name using getifaddress
    Interface(const std::string& name);
};

enum struct ArpType {
    Request,
    Reply,
    RRequest,
    RReply,
    InRequest,
    InReply,
    NAK
};

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
struct ArpFrame {
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

#endif // _FUNDAMENTAL_H