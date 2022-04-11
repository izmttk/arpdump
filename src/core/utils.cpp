#include "utils.h"

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include "Logger.h"

ArpFrame make_arp(const MacAddr& sender_mac, const IpAddr& sender_ip,
                          const MacAddr& target_mac, const IpAddr& target_ip, ArpType type) {
    ArpFrame frame;
    memset(&frame, 0, sizeof(frame));

    if(type == ArpType::Request) {
        memset(frame.eth_hdr.ether_dhost, 0xff, MAC_LEN);
    } else {
        memcpy(frame.eth_hdr.ether_dhost, target_mac.addr, MAC_LEN);
    }
    memcpy(frame.eth_hdr.ether_shost, sender_mac.addr, MAC_LEN);
    frame.eth_hdr.ether_type = htons(ETHERTYPE_ARP);
    
    frame.htype = htons(ARPHRD_ETHER);
    frame.ptype = htons(0x0800);
    frame.hlen = 0x06;
    frame.plen = 0x04;
    if(type == ArpType::Request) {
        frame.oper = htons(ARPOP_REQUEST);
    } else if (type == ArpType::Reply) {
        frame.oper = htons(ARPOP_REPLY);
    }
    memcpy(frame.sha, sender_mac.addr, MAC_LEN);
    memcpy(frame.spa, sender_ip.addr, IP_LEN);
    memcpy(frame.tha, target_mac.addr, MAC_LEN);
    memcpy(frame.tpa, target_ip.addr, IP_LEN);

    return frame;
}

ArpFrame make_arp_request(const MacAddr& sender_mac, const IpAddr& sender_ip,
                          const IpAddr& target_ip) {
    return make_arp(sender_mac, sender_ip, MacAddr(), target_ip, ArpType::Request);
}

ArpFrame make_arp_reply(const MacAddr& sender_mac, const IpAddr& sender_ip,
                        const MacAddr& target_mac, const IpAddr& target_ip) {
    return make_arp(sender_mac, sender_ip, target_mac, target_ip, ArpType::Reply);
}

void dump_arp(const ArpFrame& arp) {
    std::string log;
    log += "\n";
    log += fmt::format("ethernet: {} --> {}\n", 
        MacAddr(arp.eth_hdr.ether_shost).to_string(), 
        MacAddr(arp.eth_hdr.ether_dhost).to_string());
    log += "arp ";
    std::uint16_t oper = ntohs(arp.oper);
    switch (oper) {
        case 0x0001:
            log += "(request): ";
            log += fmt::format("who has {}? tell {}", 
                IpAddr(arp.tpa).to_string(),
                IpAddr(arp.spa).to_string());
            break;
        case 0x0002:
            log += "(reply): ";
            log += fmt::format("{} is at {}", 
                IpAddr(arp.spa).to_string(), 
                MacAddr(arp.sha).to_string());
            break;
    }
    Logger::info(log);
}

void dump_interface(const Interface& interface) {
    std::string log;
    log += "\n";
    log += fmt::format("interface: {}\n", interface.name);
    log += fmt::format("    mac: {}\n", interface.mac_addr.to_string());
    log += fmt::format("    ip: {}\n", interface.ip_addr.to_string());
    log += fmt::format("    mask: {}\n", interface.netmask.to_string());
    log += fmt::format("    broadcast: {}", interface.boardcast_addr.to_string());
    Logger::info(log);
}


// Copyright (c) 2013 Javier
// from https://gist.github.com/javiermon/6272065
IpAddr get_gateway(const Interface& interface)
{
    #define BUFFER_SIZE 4096

    IpAddr gateway;

    int     received_bytes = 0, msg_len = 0, route_attribute_len = 0;
    int     sock = -1, msgseq = 0;
    struct  nlmsghdr *nlh, *nlmsg;
    struct  rtmsg *route_entry;
    // This struct contain route attributes (route type)
    struct  rtattr *route_attribute;
    char    gateway_address[INET_ADDRSTRLEN], ifname[IF_NAMESIZE];
    char    msgbuf[BUFFER_SIZE], buffer[BUFFER_SIZE];
    char    *ptr = buffer;
    struct timeval tv;

    if ((sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0) {
        throw std::runtime_error("socket failed");
    }

    memset(msgbuf, 0, sizeof(msgbuf));
    memset(gateway_address, 0, sizeof(gateway_address));
    memset(ifname, 0, sizeof(ifname));
    memset(buffer, 0, sizeof(buffer));

    /* point the header and the msg structure pointers into the buffer */
    nlmsg = (struct nlmsghdr *)msgbuf;

    /* Fill in the nlmsg header*/
    nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlmsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
    nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlmsg->nlmsg_seq = msgseq++; // Sequence of the message packet.
    nlmsg->nlmsg_pid = getpid(); // PID of process sending the request.

    /* 1 Sec Timeout to avoid stall */
    tv.tv_sec = 1;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    /* send msg */
    if (send(sock, nlmsg, nlmsg->nlmsg_len, 0) < 0) {
        throw std::runtime_error("send failed");
    }

    /* receive response */
    do
    {
        received_bytes = recv(sock, ptr, sizeof(buffer) - msg_len, 0);
        if (received_bytes < 0) {
            throw std::runtime_error("Error in recv");
        }

        nlh = (struct nlmsghdr *) ptr;

        /* Check if the header is valid */
        if((NLMSG_OK(nlmsg, received_bytes) == 0) ||
           (nlmsg->nlmsg_type == NLMSG_ERROR))
        {
            throw std::runtime_error("Error in received packet");
        }

        /* If we received all data break */
        if (nlh->nlmsg_type == NLMSG_DONE)
            break;
        else {
            ptr += received_bytes;
            msg_len += received_bytes;
        }

        /* Break if its not a multi part message */
        if ((nlmsg->nlmsg_flags & NLM_F_MULTI) == 0)
            break;
    }
    while ((nlmsg->nlmsg_seq != (unsigned int)msgseq) || (nlmsg->nlmsg_pid != (unsigned int)getpid()));

    /* parse response */
    for ( ; NLMSG_OK(nlh, received_bytes); nlh = NLMSG_NEXT(nlh, received_bytes))
    {
        /* Get the route data */
        route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

        /* We are just interested in main routing table */
        if (route_entry->rtm_table != RT_TABLE_MAIN)
            continue;

        route_attribute = (struct rtattr *) RTM_RTA(route_entry);
        route_attribute_len = RTM_PAYLOAD(nlh);

        /* Loop through all attributes */
        for ( ; RTA_OK(route_attribute, route_attribute_len);
              route_attribute = RTA_NEXT(route_attribute, route_attribute_len))
        {
            switch(route_attribute->rta_type) {
            case RTA_OIF:
                if_indextoname(*(int *)RTA_DATA(route_attribute), ifname);
                break;
            case RTA_GATEWAY:
                inet_ntop(AF_INET, RTA_DATA(route_attribute),
                          gateway_address, sizeof(gateway_address));
                break;
            default:
                break;
            }
        }

        if ((*gateway_address) && (*ifname)) {
            if (strcmp(ifname, interface.name.c_str()) == 0) {
                gateway = IpAddr(gateway_address);
                break;
            }
        }
    }
    close(sock);
    return gateway;
}