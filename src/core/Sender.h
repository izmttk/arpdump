#pragma once

#ifndef _SENDER_H
#define _SENDER_H 1

#include "fundamental.h"
#include <string>
#include <mutex>

// singleton
class Sender {
private:
    static Sender* instance;
    static std::mutex mutex;

    int fd;
    Interface* interface;

    Sender();
    ~Sender();

public:
    Sender(const Sender&) = delete;
    Sender& operator=(const Sender&) = delete;
    Sender(Sender&&) = delete;
    Sender& operator=(Sender&&) = delete;
    static Sender& get_instance();
    void set_interface(const std::string& interface_name);
    void set_interface(const Interface& interface);
    Interface* get_interface() const;
    void send(const ArpFrame& arp); 
};

#endif // _SENDER_H