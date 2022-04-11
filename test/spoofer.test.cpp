#include "../src/Spoofer.h"

int main() {
    Spoofer spoofer("wlo1", std::chrono::seconds(5));
    spoofer.spoof(IpAddr("192.168.137.179"));
    return 0;
}