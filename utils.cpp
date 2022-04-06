#include "utils.h"

arp_frame make_arp_request(ether_addr *sha, in_addr *spa, ether_addr *tha, in_addr *tpa) {
    arp_frame arp_req;
    memcpy(arp_req.eth_hdr.ether_shost, (uint8_t *) sha, ETH_ALEN);
    memset(arp_req.eth_hdr.ether_dhost, 0xff, ETH_ALEN);
    // memcpy(arp_req.eth_hdr.ether_dhost, (uint8_t *) ether_aton("00:15:5d:4d:08:bd"), ETH_ALEN);
    arp_req.eth_hdr.ether_type = htons(ETHERTYPE_ARP);
    arp_req.htype = htons(ARPHRD_ETHER);
    arp_req.ptype = htons(0x0800); // IPv4
    arp_req.hlen  = 6;
    arp_req.plen  = 4;
    arp_req.oper  = htons(ARPOP_REQUEST);
    memcpy(arp_req.sha, sha, sizeof(arp_req.sha));
    memcpy(arp_req.spa, spa, sizeof(arp_req.spa));
    memcpy(arp_req.tha, tha, sizeof(arp_req.tha));
    memcpy(arp_req.tpa, tpa, sizeof(arp_req.tpa));
    return arp_req;
}

arp_frame make_arp_reply(ether_addr *sha, in_addr *spa, ether_addr *tha, in_addr *tpa) {
    arp_frame arp_req;
    memcpy(arp_req.eth_hdr.ether_shost, (uint8_t *) sha, ETH_ALEN);
    memcpy(arp_req.eth_hdr.ether_dhost, (uint8_t *) tha, ETH_ALEN);
    // memcpy(arp_req.eth_hdr.ether_dhost, (uint8_t *) ether_aton("00:15:5d:4d:08:bd"), ETH_ALEN);
    arp_req.eth_hdr.ether_type = htons(ETHERTYPE_ARP);
    arp_req.htype = htons(ARPHRD_ETHER);
    arp_req.ptype = htons(0x0800); // IPv4
    arp_req.hlen  = 6;
    arp_req.plen  = 4;
    arp_req.oper  = htons(ARPOP_REPLY);
    memcpy(arp_req.sha, sha, sizeof(arp_req.sha));
    memcpy(arp_req.spa, spa, sizeof(arp_req.spa));
    memcpy(arp_req.tha, tha, sizeof(arp_req.tha));
    memcpy(arp_req.tpa, tpa, sizeof(arp_req.tpa));
    return arp_req;
}

sockaddr_ll get_addr_by_name(const char* ifr) {
    sockaddr_ll device;
    memset(&device, 0, sizeof(device));

    // https://man7.org/linux/man-pages/man7/packet.7.html
    // 设置索引即可，其他保持默认
    device.sll_family  = AF_PACKET;
    device.sll_ifindex = if_nametoindex(ifr);
    
    if (device.sll_ifindex == 0) {
        perror("if_nametoindex() failed to obtain interface index");
        exit(EXIT_FAILURE);
    }
    return device;
}

void dump_arp(arp_frame *arp) {
    uint16_t htype = ntohs(arp->htype);
    uint16_t ptype = ntohs(arp->ptype);
    uint16_t oper = ntohs(arp->oper);
    switch (htype) {
        case 0x0001:
            printf("ARP HTYPE: Ethernet(0x%04X)\n", htype);
            break;
        default:
            printf("ARP HYPE: 0x%04X\n", htype);
            break;
    }
    switch (ptype) {
        case 0x0800:
            printf("ARP PTYPE: IPv4(0x%04X)\n", ptype);
            break;
        default:
            printf("ARP PTYPE: 0x%04X\n", ptype);
            break;
    }
    printf("ARP HLEN: %d\n", arp->hlen);
    printf("ARP PLEN: %d\n", arp->plen);
    switch (oper) {
        case 0x0001:
            printf("ARP OPER: Request(0x%04X)\n", oper);
            break;
        case 0x0002:
            printf("ARP OPER: Response(0x%04X)\n", oper);
            break;
        default:
            printf("ARP OPER: 0x%04X\n", oper);
            break;
    }
    printf("ARP Sender HA: %02X:%02X:%02X:%02X:%02X:%02X\n", arp->sha[0],
           arp->sha[1], arp->sha[2], arp->sha[3], arp->sha[4], arp->sha[5]);
    printf("ARP Sender PA: %d.%d.%d.%d\n", arp->spa[0], arp->spa[1],
           arp->spa[2], arp->spa[3]);
    printf("ARP Target HA: %02X:%02X:%02X:%02X:%02X:%02X\n", arp->tha[0],
           arp->tha[1], arp->tha[2], arp->tha[3], arp->tha[4], arp->tha[5]);
    printf("ARP Target PA: %d.%d.%d.%d\n", arp->tpa[0], arp->tpa[1],
           arp->tpa[2], arp->tpa[3]);
    printf("ARP DONE =====================\n");
}



void send_arp_to(const int fd, const char* if_name, arp_frame* arp_send) {
    sockaddr_ll device = get_addr_by_name(if_name);
    ssize_t size = sendto(fd, arp_send, ETH_ARP_LEN, 0, (sockaddr *) &device, sizeof(device));
    if(size == -1) {
        perror("socket send error");
        exit(EXIT_FAILURE);
    }
}

void recv_arp_from(const int fd, const char* if_name, std::function<void(arp_frame *, sockaddr_ll *)>callback) {
    if(strcmp(if_name, "any") != 0) {
        sockaddr_ll device = get_addr_by_name(if_name);
        if (bind(fd, (sockaddr *)&device, sizeof(device)) == -1) {
            perror("socket bind error");
            exit(EXIT_FAILURE);
        }
    }

    uint8_t buf[1024];
    memset(buf, 0, sizeof(buf));
    sockaddr_ll device;
    socklen_t   device_len;
    while(1) {
        ssize_t size = recvfrom(fd, buf, sizeof(buf), 0, (sockaddr *) &device, &device_len);
        if(size == -1) {
            perror("socket receive error");
            exit(EXIT_FAILURE);
        }
        if(size < ETH_ARP_LEN) {
            continue;
        }
        arp_frame * eth_arp = (arp_frame *) buf;
        if(eth_arp->eth_hdr.ether_type != htons(ETHERTYPE_ARP)) {
            continue;
        }
        callback(eth_arp, &device);
    }
}
