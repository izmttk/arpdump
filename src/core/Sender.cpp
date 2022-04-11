#include "Sender.h"
#include "utils.h"
#include <unistd.h>
#include "Logger.h"

Sender *Sender::instance = nullptr;
std::mutex Sender::mutex;

Sender::Sender(): fd(-1), interface(nullptr) {}

Sender::~Sender() {
    if (fd != -1) {
        close(fd);
    }
    if(interface != nullptr) {
        delete interface;
    }
}

Sender& Sender::get_instance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new Sender();
            Logger::debug("Sender instance created.");
            instance->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
            Logger::debug("Sender socket fd: {}.", instance->fd);
            if(instance->fd == -1) {
                throw std::runtime_error("please run the program as root user");
            }
        }
    }
    return *instance;
}

Interface* Sender::get_interface() const {
    return this->interface;
}

void Sender::set_interface(const std::string& interface_name) {
    if(this->interface != nullptr) {
        delete this->interface;
    }
    this->interface = new Interface(interface_name);
    Logger::debug("Sender interface set to {}.", interface_name);
}

void Sender::set_interface(const Interface& interface) {
    if(this->interface != nullptr) {
        delete this->interface;
    }
    this->interface = new Interface(interface);
    Logger::debug("Sender interface set to {}.", interface.name);
}

void Sender::send(const ArpFrame& arp) {
    Logger::debug("Sender start.");
    if(this->interface == nullptr) {
        throw std::runtime_error("please set interface first");
    }
    // get interface index
    sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(this->interface->name.c_str());

    if(sll.sll_ifindex == 0) {
        throw std::runtime_error("interface not found");
    }
    Logger::debug("Sender interface index: {}.", sll.sll_ifindex);

    ssize_t size = sendto(this->fd, (void *) &arp, ETH_ARP_LEN, 0, (sockaddr *) &sll, sizeof(sll));
    if(size == -1) {
        throw std::runtime_error("send failed");
    }
}
