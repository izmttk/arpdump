#pragma once

#ifndef _FORWARDER_H
#define _FORWARDER_H 1

#include "fundamental.h"
#include "Receiver.h"
#include "Sender.h"
#include "utils.h"

class Forwarder {
private:
    int fd;
    Interface interface;
    bool is_running;
public:
    Forwarder() = delete;
    Forwarder(const std::string& interface_name);
    Forwarder(const Interface& interface);
    void set_interface(Interface& interface);
    void set_interface(std::string& interface_name);
    Interface get_interface() const;
    void start(const MacAddr& from, const MacAddr& to);
    void stop();
};

#endif // _FORWARDER_H