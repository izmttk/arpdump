#include "Receiver.h"
#include "utils.h"
#include <unistd.h>
#include <spdlog/spdlog.h>
#include "Logger.h"

Receiver *Receiver::instance = nullptr;
std::mutex Receiver::mutex;

Receiver::Receiver(): fd(-1), is_running(false), once(false), timeout(0),interface(nullptr) {}

Receiver::~Receiver() {
    if (fd != -1) {
        close(fd);
    }
    if(interface != nullptr) {
        delete interface;
    }
}

Receiver& Receiver::get_instance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new Receiver();
            Logger::debug("Receiver instance created.");
            instance->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
            Logger::debug("Receiver socket fd: {}.", instance->fd);
            if(instance->fd == -1) {
                throw std::runtime_error("please run the program as root user");
            }
        }
    }
    return *instance;
}

Interface* Receiver::get_interface() const {
    return this->interface;
}

void Receiver::set_interface(const std::string& interface_name) {
    if(this->interface != nullptr) {
        delete this->interface;
    }
    this->interface = new Interface(interface_name);
    Logger::debug("Receiver interface set to {}.", interface_name);
}

void Receiver::set_interface(const Interface& interface) {
    if(this->interface != nullptr) {
        delete this->interface;
    }
    this->interface = new Interface(interface);
    Logger::debug("Receiver interface set to {}.", interface.name);
}
void Receiver::set_once(bool once) {
    this->once = once;
}

bool Receiver::is_once() const {
    return this->once;
}

void Receiver::set_timeout(const std::chrono::seconds& timeout) {
    this->timeout = timeout;
}

std::chrono::seconds Receiver::get_timeout() const {
    return this->timeout;
}

void Receiver::start(std::function<void(const ArpFrame&)> callback) {
    if(this->is_running) {
        Logger::debug("Receiver is already running.");
    }

    Logger::debug("Receiver start.");
    if(this->interface == nullptr) {
        throw std::runtime_error("please set interface first");
    }
    // get interface index
    sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(this->interface->name.c_str());
    sll.sll_protocol = htons(ETH_P_ALL);

    if(sll.sll_ifindex == 0) {
        throw std::runtime_error("interface not found");
    }
    Logger::debug("Receiver interface index: {}.", sll.sll_ifindex);


    // set promiscuous mode
    struct packet_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.mr_ifindex = sll.sll_ifindex;
    mreq.mr_type = PACKET_MR_PROMISC;
    if(setsockopt(this->fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
        throw std::runtime_error("failed to set promiscuous mode");
    }
    Logger::debug("Receiver set promiscuous mode.");

    if (bind(this->fd, (sockaddr *)&sll, sizeof(sll)) == -1) {
        throw std::runtime_error("bind error");
    }

    uint8_t buf[ETH_FRAME_LEN];
    memset(buf, 0, sizeof(buf));

    this->is_running = true;

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    while (true) {
        if(this->timeout != std::chrono::seconds(0)) {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            if(end - start > this->timeout) {
                break;
            }
        }

        ssize_t size = recvfrom(this->fd, buf, sizeof(buf), 0, NULL, NULL);
        if(size == -1) {
            this->is_running = false;
            throw std::runtime_error("socket receive error");
        }
        Logger::debug("Receiver received {} bytes.", size);
        if(size != ETH_ARP_LEN) {
            continue;
        }
        ArpFrame eth_arp = *((ArpFrame *) buf);
        if(eth_arp.eth_hdr.ether_type != htons(ETHERTYPE_ARP)) {
            continue;
        }
        callback(eth_arp);

        if(!this->is_running) break;
        if(this->once) break;
    }
}

void Receiver::stop() {
    this->is_running = false;
}
