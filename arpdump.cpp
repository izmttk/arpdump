#include "arpdump.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
// using namespace std;

#include "sniffer.h"
#include "attacker.h"
#include "cmdline.h"

int main(int argc, char *argv[]) {
    cmdline::parser parser;
    parser.add<std::string>("interface", 'i', "interface to use", true);
    parser.add("send", 's', "send mode");
    parser.add("sniff", 'w', "sniff mode");
    parser.add("attack", 'a', "attack mode");
    parser.add("netcut", 'c', "use netcut in attack mode");
    parser.add("reply", 'r', "send arp reply in send mode");
    parser.add<std::string>("dest-ip", 'D', "ip address of the device to send in send or attack mode", false);
    parser.add<std::string>("dest-mac", 'X', "ether address of the device to send in send or attack mode", false);
    parser.add<std::string>("src-ip", 'S', "sender ip in arp packet in send or attack mode", false);
    parser.add<std::string>("src-mac", 'C', "sender ether addr in arp packet in send or attack mode", false);
    parser.add<int>("interval-time", 'T', "inverval time to send packet in attack mode", false, 10);

    parser.parse_check(argc, argv);


    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(sockfd == -1) {
        perror("please run the program as root user");
        return EXIT_FAILURE;
    }
    printf("fd: %d\n", sockfd);

    std::string if_name = parser.get<std::string>("interface");

    if(parser.exist("sniff")) {
        std::thread t(sniffer_thread, sockfd, if_name);
        t.join();
    } else if(parser.exist("send")) {
        std::string type = parser.get<std::string>("type");
        std::string dest_ip = parser.get<std::string>("dest-ip");
        std::string dest_mac = parser.get<std::string>("dest-mac");
        std::string src_ip = parser.get<std::string>("src-ip");
        std::string src_mac = parser.get<std::string>("src-mac");

        // if(parser.get<bool>("reply")) {
        //     send_arp_to(sockfd, if_name.c_str(), make_arp_reply());
        // } else {
        //     send_arp_to(sockfd, if_name.c_str(), make_arp_request());
        // }

    } else if(parser.exist("attack")) {
        std::string type;
        if(parser.get<bool>("netcut")) {
            type = "netcut";
        } else {
            type = "intercept";
        }
        std::string dest_ip = parser.get<std::string>("dest-ip");
        std::string dest_mac = parser.get<std::string>("dest-mac");
        std::string src_ip = parser.get<std::string>("src-ip");
        std::string src_mac;
        if(!parser.exist("netcut")) {
            std::string src_mac = parser.get<std::string>("src-mac");
        }
        int interval = parser.get<int>("interval-time");

        std::thread t(
            attacker_thread,
            sockfd, 
            if_name,
            type,       // surveil mode
            dest_mac,   // victim_mac
            dest_ip,    // victim ip
            src_mac,    // self mac
            src_ip,     // pretending ip
            interval
        );
        t.join();
    }
    shutdown(sockfd, SHUT_RDWR);
    return EXIT_SUCCESS;
}