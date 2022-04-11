#pragma once
#ifndef _SPOOFER_H
#define _SPOOFER_H 1

#include "core/fundamental.h"
#include "core/Receiver.h"
#include "core/Sender.h"
#include "core/Forwarder.h"
#include "core/utils.h"
#include <chrono>

class Spoofer {
private:
    Interface interface;
    Receiver& receiver;
    Sender& sender;
    std::chrono::seconds interval;
    bool is_running;

public:
    Spoofer() = delete;
    Spoofer(const std::string& interface_name, const std::chrono::seconds& interval);
    Spoofer(const Interface& interface, const std::chrono::seconds& interval);
    void set_interface(Interface& interface);
    void set_interface(std::string& interface_name);
    Interface get_interface() const;
    void set_interval(const std::chrono::seconds& interval);
    std::chrono::seconds get_interval() const;
    void spoof(const ArpFrame& arp);
    // tell spoof_ip/mac target_ip is at target_mac
    void spoof(
        const MacAddr& spoofed_mac, 
        const IpAddr& spoofed_ip,
        const MacAddr& target_mac,
        const IpAddr& target_ip);
    // tell spoofed_ip target_ip is at target_mac
    void spoof(
        const IpAddr& spoofed_ip, 
        const MacAddr& target_mac,
        const IpAddr& target_ip);
    // tell spoofed_ip target_ip is at this host's mac
    void spoof(
        const IpAddr& spoofed_ip, 
        const IpAddr& target_ip);
    // tell spoofed_ip gateway ip is at this host's mac
    void spoof(const IpAddr& spoofed_ip);
    void stop();
};

#endif // _SPOOFER_H