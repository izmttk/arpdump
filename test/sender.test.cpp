#include "../src/core/fundamental.h"
#include "../src/core/Sender.h"
#include "../src/core/utils.h"

int main() {
    Sender& sender = Sender::get_instance();
    Interface interface("wlo1");
    sender.set_interface(interface);
    dump_interface(interface);

    ArpFrame arp = make_arp_request(
        interface.mac_addr,
        interface.ip_addr,
        IpAddr("192.168.137.123")
    );
    dump_arp(arp);
    sender.send(arp);
    return 0;
}