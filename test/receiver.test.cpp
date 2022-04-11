#include "../src/core/fundamental.h"
#include "../src/core/Receiver.h"
#include "../src/core/utils.h"

int main() {
    Receiver& receiver = Receiver::get_instance();
    receiver.set_interface("wlo1");
    receiver.start([](const ArpFrame& frame) {
        dump_arp(frame);
    });
    return 0;
}