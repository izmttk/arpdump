#pragma once

#ifndef _UTILS_H
#define _UTILS_H 1

#include "fundamental.h"
#include <cstdint>

ArpFrame make_arp(const MacAddr& sender_mac, const IpAddr& sender_ip,
                          const MacAddr& target_mac, const IpAddr& target_ip, ArpType type);

ArpFrame make_arp_request(const MacAddr& sender_mac, const IpAddr& sender_ip,
                          const IpAddr& target_ip);

ArpFrame make_arp_reply(const MacAddr& sender_mac, const IpAddr& sender_ip,
                        const MacAddr& target_mac, const IpAddr& target_ip);

void dump_arp(const ArpFrame& arp);

void dump_interface(const Interface& interface);

IpAddr get_gateway(const Interface& interface);

#endif // _UTILS_H