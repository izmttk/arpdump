#pragma once

#ifndef _SNIFFER_H
#define _SNIFFER_H 1

#include <functional>

#include "core/fundamental.h"
#include "core/Receiver.h"

class Sniffer {
private:
    Receiver& receiver;
    size_t count = 0;
    Interface interface;
    
public:
    Sniffer() = delete;
    Sniffer(const std::string& interface_name);
    Sniffer(const Interface& interface);
    void set_interface(Interface& interface);
    void set_interface(std::string& interface_name);
    Interface get_interface() const;
    size_t get_count() const;
    void sniff();
    void stop();
};

#endif // _SNIFFER_H