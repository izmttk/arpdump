#pragma once

#ifndef _RECIEVER_H
#define _RECIEVER_H 1

#include "fundamental.h"
#include <mutex>
#include <functional>

// singleton
class Receiver {
private:
    static Receiver* instance;
    static std::mutex mutex;
    
    int fd;
    bool is_running;
    bool once;
    std::chrono::seconds timeout;
    Interface* interface;

    Receiver();
    ~Receiver();

public:
    Receiver(const Receiver&) = delete;
    Receiver& operator=(const Receiver&) = delete;
    Receiver(Receiver&&) = delete;
    Receiver& operator=(Receiver&&) = delete;
    static Receiver& get_instance();
    void set_interface(const std::string& interface_name);
    void set_interface(const Interface& interface);
    Interface* get_interface() const;
    void set_once(bool once);
    bool is_once() const;
    void set_timeout(const std::chrono::seconds& timeout);
    std::chrono::seconds get_timeout() const;
    void start(std::function<void(const ArpFrame&)> callback); 
    void stop();
};

#endif // _RECIEVER_H

