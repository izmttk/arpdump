#include "Sniffer.h"
#include "core/utils.h"
#include <spdlog/spdlog.h>
#include "core/Logger.h"

Sniffer::Sniffer(const std::string& interface_name)
:receiver(Receiver::get_instance()), count(0), interface(interface_name) {
    this->set_interface(this->interface);
}

Sniffer::Sniffer(const Interface& interface)
:receiver(Receiver::get_instance()), count(0), interface(interface) {
    this->set_interface(this->interface);
}

Interface Sniffer::get_interface() const {
    return this->interface;
}

void Sniffer::set_interface(Interface& interface) {
    this->interface = interface;
    Interface* rec_if = receiver.get_interface();
    if(rec_if == nullptr || rec_if->name != interface.name) {
        receiver.set_interface(interface);
    }
    Logger::debug("Sniffer interface set to {}.", interface.name);
}

void Sniffer::set_interface(std::string& interface_name) {
    Interface interface(interface_name);
    this->set_interface(interface);
}

size_t Sniffer::get_count() const {
    return this->count;
}

void Sniffer::sniff() {
    Logger::info("starting sniff mode.");
    this->count = 0;
    receiver.start([this](const ArpFrame& frame) {
        this->count++;
        Logger::info("receive arp {}.", this->count);
        dump_arp(frame);
    });
}

void Sniffer::stop() {
    receiver.stop();
}