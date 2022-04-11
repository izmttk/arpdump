#include "../src/core/fundamental.h"
#include "../src/core/utils.h"

int main() {
    Interface interface("wlo1");
    dump_interface(interface);
    IpAddr gateway = get_gateway(interface);
    printf("gateway: %s\n", gateway.to_string().c_str());
    return 0;
}