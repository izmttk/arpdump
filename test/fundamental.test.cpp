#include "../src/core/fundamental.h"
#include "../src/core/utils.h"
#include <string>
#include <iostream>

int main() {
    IpAddr ip_addr("192.168.137.1");
    MacAddr mac_addr("11:22:33:44:55:66");
    std::cout << ip_addr.to_string() << std::endl;
    std::cout << mac_addr.to_string() << std::endl;
    return 0;
}