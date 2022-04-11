#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "Sniffer.h"
#include "Spoofer.h"
#include "core/Logger.h"



int main(int argc, char *argv[]) {

    CLI::App app{"arp sniffing and spoofing tool"};
    std::string interface;
    bool sniff = true;
    bool spoof = false;
    std::string spoofed_ip;
    std::string spoofed_mac;
    std::string target_ip;
    std::string target_mac;
    int interval = 10;
    std::string log_file;
    bool verbose = false;

    app.add_option("-i,--interface", interface, "interface to use")
        ->required();
    CLI::Option* sniff_op =  app.add_flag("-s,--sniff", sniff, "sniff mode, default");
    CLI::Option* spoof_op =  app.add_flag("-c,--spoof", spoof, "spoof mode")
        ->excludes(sniff_op);
    app.add_option("spoofed-ip", spoofed_ip, "ip of host you want to spoof")
        ->needs(spoof_op);
    app.add_option("-m,--spoofed-mac", spoofed_mac, "mac of host you want to spoof")
        ->needs(spoof_op);
    app.add_option("-n,--target-ip", target_ip, "tell other one which ip I have")
        ->needs(spoof_op);
    app.add_option("-e,--target-mac", target_mac, "tell other one which mac I have")
        ->needs(spoof_op);
    app.add_option("-t,--interval", interval, "inverval time (second) to send frame")
        ->needs(spoof_op);

    app.add_option("-l,--log", log_file, "specify log file");
    app.add_flag("-v,--verbose", verbose, "verbose mode");

    CLI11_PARSE(app, argc, argv);

    Logger::console_logger_init();
    if(!log_file.empty()) {
        Logger::file_logger_init(log_file);
    }
    if(verbose) {
        Logger::set_level(spdlog::level::debug);
    }

    if(spoof) {

        Spoofer spoofer(interface, std::chrono::seconds(interval));
        if(!spoofed_mac.empty() && !spoofed_ip.empty() && !target_ip.empty() && !target_mac.empty()) {
            spoofer.spoof(
                MacAddr(spoofed_mac),
                IpAddr(spoofed_ip),
                MacAddr(target_mac),
                IpAddr(target_ip)
            );
        } else if (!spoofed_ip.empty() && !target_mac.empty() && !target_ip.empty()) {
            spoofer.spoof(
                IpAddr(spoofed_ip),
                MacAddr(target_mac),
                IpAddr(target_ip)
            );
        } else if (!target_ip.empty() && !target_ip.empty()) {
            spoofer.spoof(
                IpAddr(spoofed_ip),
                IpAddr(target_ip)
            );
        } else if (!spoofed_ip.empty()) {
            spoofer.spoof(
                IpAddr(spoofed_ip)
            );
        } else {
            Logger::error("illegal ip and mac arguments combination");
            return EXIT_FAILURE;
        }
    } else {
        Sniffer sniffer(interface);
        sniffer.sniff();
    }
    return EXIT_SUCCESS;
}