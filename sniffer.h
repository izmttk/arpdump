#pragma once

#ifndef _SNIFFER_H
#define _SNIFFER_H 1

#include <net/if.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

void listen(const int fd, const std::string& if_name);

void sniffer_thread(const int fd, const std::string& if_name);

#endif