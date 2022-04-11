#include "Forwarder.h"
#include "Logger.h"
#include <spdlog/spdlog.h>

Forwarder::Forwarder(const std::string& interface_name)
:fd(-1),
interface(interface_name),
is_running(false) {
    this->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    Logger::debug("Forwarder socket fd: {}.", this->fd);
    if(this->fd == -1) {
        throw std::runtime_error("please run the program as root user");
    }
}

Forwarder::Forwarder(const Interface& interface)
:fd(-1),
interface(interface),
is_running(false) {
    this->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    Logger::debug("Forwarder socket fd: {}.", this->fd);
    if(this->fd == -1) {
        throw std::runtime_error("please run the program as root user");
    }
}

void Forwarder::set_interface(Interface& interface) {
    this->interface = interface;
    Logger::debug("Forwarder interface set to {}.", interface.name);
}


void Forwarder::set_interface(std::string& interface_name) {
    Interface interface(interface_name);
    this->set_interface(interface);
}

Interface Forwarder::get_interface() const {
    return this->interface;
}

void Forwarder::start(const MacAddr& from, const MacAddr& to) {
    if(this->is_running) {
        throw std::runtime_error("Forwarder is already running.");
    }
    
    Logger::debug("Forwarder start.");

    // get interface index
    sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(this->interface.name.c_str());
    sll.sll_protocol = htons(ETH_P_ALL);

    if(sll.sll_ifindex == 0) {
        throw std::runtime_error("interface not found");
    }
    Logger::debug("Forwarder interface index: {}.", sll.sll_ifindex);


    if (bind(this->fd, (sockaddr *)&sll, sizeof(sll)) == -1) {
        throw std::runtime_error("bind error");
    }

    uint8_t buf[ETH_FRAME_LEN];
    memset(buf, 0, sizeof(buf));

    this->is_running = true;


    while (true) {
        ssize_t size = recvfrom(this->fd, buf, sizeof(buf), 0, NULL, NULL);
        if(size == -1) {
            this->is_running = false;
            throw std::runtime_error("socket receive error");
        }
        Logger::debug("Forwarder received {} bytes.", size);
        if(size < ETH_HLEN) {
            continue;
        }
        ether_header* eth_frame = (ether_header *) buf;
        if(from == MacAddr(eth_frame->ether_shost)) {
            memcpy(eth_frame->ether_shost, this->interface.mac_addr.addr, MAC_LEN);
            memcpy(eth_frame->ether_dhost, to.addr, MAC_LEN);
            if(sendto(this->fd, buf, size, 0, (sockaddr *)&sll, sizeof(sll)) == -1) {
                this->is_running = false;
                throw std::runtime_error("socket send error");
            }
            Logger::debug("Forwarder sent frame from %s to %s.", from.to_string(), to.to_string());
        }

        if(!this->is_running) break;
    }
}

void Forwarder::stop() {
    this->is_running = false;
}
