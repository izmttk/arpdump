#pragma once
#ifndef _FUNDAMENTAL_H
#define _FUNDAMENTAL_H 1

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>

#include <sys/socket.h>
#include <sys/ioctl.h>

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
    MacAddr():addr{0} {}
    MacAddr(const MacAddr &mac) {
        memcpy(addr, mac.addr, MAC_LEN);
    }
    MacAddr(const uint8_t* new_addr) {
        memcpy(addr, new_addr, MAC_LEN);
    }
    MacAddr(const ether_addr& eth) {
        memcpy(addr, &eth, MAC_LEN);
    }
    MacAddr(const sockaddr_ll& sll) {
        memcpy(addr, sll.sll_addr, MAC_LEN);
    }
    MacAddr(const std::string& mac_str) {
        // network byte order
        std::stringstream ss(mac_str);
        std::string token;
        int i = 0;
        while (std::getline(ss, token, ':')) {
            addr[i++] = std::stoi(token, nullptr, 16);
        }
        if (i != MAC_LEN) {
            throw std::runtime_error("invalid mac address");
        }
    }
    std::string to_string() const {
        std::stringstream ss;
        // hex string format
        for (int i = 0; i < MAC_LEN; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)addr[i];
            if (i != MAC_LEN - 1) {
                ss << ":";
            }
        }
        return ss.str();
    }
    bool operator == (const MacAddr& mac) const {
        return memcmp(addr, mac.addr, MAC_LEN) == 0;
    }
    operator ether_addr() const {
        ether_addr eth;
        memcpy(&eth, addr, MAC_LEN);
        return eth;
    }
};

class IpAddr {
public:
    std::uint8_t addr[IP_LEN];
    IpAddr():addr{0} {}
    IpAddr(const IpAddr& ip) {
        memcpy(addr, ip.addr, IP_LEN);
    }
    IpAddr(const uint32_t new_addr) {
        memcpy(addr, &new_addr, IP_LEN);
    }
    IpAddr(const uint8_t* new_addr) {
        memcpy(addr, new_addr, IP_LEN);
    }
    IpAddr(const in_addr& in) {
        uint32_t ip = in.s_addr;
        memcpy(addr, &ip, IP_LEN);
    }
    IpAddr(const sockaddr_in& sin) {
        uint32_t ip = sin.sin_addr.s_addr;
        memcpy(addr, &ip, IP_LEN);
    }
    IpAddr(const std::string& ip_str) {
        // parse ip string using dot as delimiter
        // network byte order
        std::stringstream ss(ip_str);
        std::string token;
        int i = 0;
        while (std::getline(ss, token, '.')) {
            addr[i++] = (std::uint8_t)std::stoi(token, nullptr, 10);
        }
        if (i != IP_LEN) {
            throw std::runtime_error("invalid ip address");
        }
    }
    std::string to_string() const {
        std::stringstream ss;
        for (int i = 0; i < IP_LEN; i++) {
            ss << std::dec << (int)addr[i];
            if (i != IP_LEN - 1) {
                ss << ".";
            }
        }
        return ss.str();
    }
    bool operator == (const IpAddr& ip) const {
        return memcmp(addr, ip.addr, IP_LEN) == 0;
    }
    operator in_addr() const {
        in_addr in;
        uint32_t ip = ntohl( * ((std::uint32_t *) addr) );
        memcpy(&in, &ip, IP_LEN);
        return in;
    }
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
    Interface(const std::string& name, const MacAddr& mac_addr, const IpAddr& ip_addr, const IpAddr& boardcast_addr, const IpAddr& netmask) {
        this->name = name;
        this->mac_addr = mac_addr;
        this->ip_addr = ip_addr;
        this->boardcast_addr = boardcast_addr;
        this->netmask = netmask;
        this->flags = 0;
    }
    Interface(const Interface& interface) {
        this->name = interface.name;
        this->mac_addr = interface.mac_addr;
        this->ip_addr = interface.ip_addr;
        this->boardcast_addr = interface.boardcast_addr;
        this->netmask = interface.netmask;
        this->flags = interface.flags;
    }
    Interface& operator=(const Interface& interface) {
        this->name = interface.name;
        this->mac_addr = interface.mac_addr;
        this->ip_addr = interface.ip_addr;
        this->boardcast_addr = interface.boardcast_addr;
        this->netmask = interface.netmask;
        this->flags = interface.flags;
        return *this;
    }

    // constructor from name using getifaddress
    Interface(const std::string& name) {
        ifaddrs *ifaddr, *ifa;
        getifaddrs(&ifaddr);
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;
            if (ifa->ifa_addr->sa_family == AF_INET) {
                if (strcmp(ifa->ifa_name, name.c_str()) == 0) {
                    this->name = name;
                    this->ip_addr = IpAddr( * ( (sockaddr_in *) ifa->ifa_addr ) );
                    this->boardcast_addr = IpAddr( * ( (sockaddr_in *) ifa->ifa_broadaddr ) );
                    this->netmask = IpAddr( * ( (sockaddr_in *) ifa->ifa_netmask ) );
                    this->flags = ifa->ifa_flags;
                }
            } else if(ifa->ifa_addr->sa_family == AF_PACKET) {
                if (strcmp(ifa->ifa_name, name.c_str()) == 0) {
                    this->mac_addr = MacAddr( * ( (sockaddr_ll *) ifa->ifa_addr ) );
                }
            }
        }
        freeifaddrs(ifaddr);
    }
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