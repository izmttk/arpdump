#include "fundamental.h"

MacAddr::MacAddr():addr{0} {}
MacAddr::MacAddr(const MacAddr &mac) {
    memcpy(addr, mac.addr, MAC_LEN);
}
MacAddr::MacAddr(const uint8_t* new_addr) {
    memcpy(addr, new_addr, MAC_LEN);
}
MacAddr::MacAddr(const ether_addr& eth) {
    memcpy(addr, &eth, MAC_LEN);
}
MacAddr::MacAddr(const sockaddr_ll& sll) {
    memcpy(addr, sll.sll_addr, MAC_LEN);
}
MacAddr::MacAddr(const std::string& mac_str) {
    // network byte order
    std::stringstream ss(mac_str);
    std::string token;
    int i = 0;
    while (std::getline(ss, token, ':')) {
        addr[i++] = std::stoi(token, nullptr, 16);
    }
    if (i != MAC_LEN) {
        throw std::runtime_error("invalid mac address");
    }
}
std::string MacAddr::to_string() const {
    std::stringstream ss;
    // hex string format
    for (int i = 0; i < MAC_LEN; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)addr[i];
        if (i != MAC_LEN - 1) {
            ss << ":";
        }
    }
    return ss.str();
}
bool MacAddr::operator == (const MacAddr& mac) const {
    return memcmp(addr, mac.addr, MAC_LEN) == 0;
}
MacAddr::operator ether_addr() const {
    ether_addr eth;
    memcpy(&eth, addr, MAC_LEN);
    return eth;
}


IpAddr::IpAddr():addr{0} {}
IpAddr::IpAddr(const IpAddr& ip) {
    memcpy(addr, ip.addr, IP_LEN);
}
IpAddr::IpAddr(const uint32_t new_addr) {
    memcpy(addr, &new_addr, IP_LEN);
}
IpAddr::IpAddr(const uint8_t* new_addr) {
    memcpy(addr, new_addr, IP_LEN);
}
IpAddr::IpAddr(const in_addr& in) {
    uint32_t ip = in.s_addr;
    memcpy(addr, &ip, IP_LEN);
}
IpAddr::IpAddr(const sockaddr_in& sin) {
    uint32_t ip = sin.sin_addr.s_addr;
    memcpy(addr, &ip, IP_LEN);
}
IpAddr::IpAddr(const std::string& ip_str) {
    // parse ip string using dot as delimiter
    // network byte order
    std::stringstream ss(ip_str);
    std::string token;
    int i = 0;
    while (std::getline(ss, token, '.')) {
        addr[i++] = (std::uint8_t)std::stoi(token, nullptr, 10);
    }
    if (i != IP_LEN) {
        throw std::runtime_error("invalid ip address");
    }
}
std::string IpAddr::to_string() const {
    std::stringstream ss;
    for (int i = 0; i < IP_LEN; i++) {
        ss << std::dec << (int)addr[i];
        if (i != IP_LEN - 1) {
            ss << ".";
        }
    }
    return ss.str();
}
bool IpAddr::operator == (const IpAddr& ip) const {
    return memcmp(addr, ip.addr, IP_LEN) == 0;
}
IpAddr::operator in_addr() const {
    in_addr in;
    uint32_t ip = ntohl( * ((std::uint32_t *) addr) );
    memcpy(&in, &ip, IP_LEN);
    return in;
}



Interface::Interface(const std::string& name, const MacAddr& mac_addr, const IpAddr& ip_addr, const IpAddr& boardcast_addr, const IpAddr& netmask) {
    this->name = name;
    this->mac_addr = mac_addr;
    this->ip_addr = ip_addr;
    this->boardcast_addr = boardcast_addr;
    this->netmask = netmask;
    this->flags = 0;
}
Interface::Interface(const Interface& interface) {
    this->name = interface.name;
    this->mac_addr = interface.mac_addr;
    this->ip_addr = interface.ip_addr;
    this->boardcast_addr = interface.boardcast_addr;
    this->netmask = interface.netmask;
    this->flags = interface.flags;
}
Interface& Interface::operator=(const Interface& interface) {
    this->name = interface.name;
    this->mac_addr = interface.mac_addr;
    this->ip_addr = interface.ip_addr;
    this->boardcast_addr = interface.boardcast_addr;
    this->netmask = interface.netmask;
    this->flags = interface.flags;
    return *this;
}

// constructor from name using getifaddress
Interface::Interface(const std::string& name) {
    ifaddrs *ifaddr, *ifa;
    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (strcmp(ifa->ifa_name, name.c_str()) == 0) {
                this->name = name;
                this->ip_addr = IpAddr( * ( (sockaddr_in *) ifa->ifa_addr ) );
                this->boardcast_addr = IpAddr( * ( (sockaddr_in *) ifa->ifa_broadaddr ) );
                this->netmask = IpAddr( * ( (sockaddr_in *) ifa->ifa_netmask ) );
                this->flags = ifa->ifa_flags;
            }
        } else if(ifa->ifa_addr->sa_family == AF_PACKET) {
            if (strcmp(ifa->ifa_name, name.c_str()) == 0) {
                this->mac_addr = MacAddr( * ( (sockaddr_ll *) ifa->ifa_addr ) );
            }
        }
    }
    freeifaddrs(ifaddr);
}
