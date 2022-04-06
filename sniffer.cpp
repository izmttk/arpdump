#include "sniffer.h"
#include "utils.h"

void listen(const int fd, const std::string& if_name) {
    int count = 0;
    recv_arp_from(fd, if_name.c_str(), [&](arp_frame *arp, sockaddr_ll *device){
        printf("%d: ", ++count);
        printf("from interface %s\n", ether_ntoa((ether_addr *) device->sll_addr));
        dump_arp(arp);
    });
}

void sniffer_thread(const int fd, const std::string& if_name) {
    listen(fd, if_name);
}
