#pragma once
#ifndef _ATTACKER_H
#define _ATTACKER_H 1

#include "utils.h"

#include <sys/socket.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <condition_variable>

void netcut(
    const int fd,
    const std::string& if_name,
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_mac,
    const std::string& pretending_ip,
    int interval_time
);

void middleman_passive(
    const int fd,
    const std::string& if_name,
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_mac,
    const std::string& pretending_ip,
    int interval_time
);

void attacker_thread(
    const int fd, 
    const std::string& if_name, 
    const std::string& type, 
    const std::string& victim_mac,
    const std::string& victim_ip,
    const std::string& pretending_mac,
    const std::string& pretending_ip,
    int interval_time = 30
);

#endif