#include "mac.h"
#include "pch.h"
#include "handler.h"
#include "macframe.h"
#include "beacon.h"


void usage()
{
    std::cout << "Usage: ./csa <interface> <ap_mac> " << std::endl;
}

int check_args(int argc, char* argv[])
{
    if (argc != 3) {
        usage();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



int main(int argc, char* argv[]) {
    if (check_args(argc, argv) == EXIT_FAILURE) {
        return EXIT_FAILURE ;
    }
    //==========Config initialization==========

    Config config;
    config.interface = argv[1];
    config.ap_mac = Mac(argv[2]);

    //==========PCAP initialization==========

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = handle_init(config.interface.c_str(), errbuf);
    if(pcap == nullptr) 
    {
        return EXIT_FAILURE;
    }
    //=========Find target AP's beacon frame=========
    
    const u_char* packet;
    int len;
    packet = find_target_beacon(pcap, config.ap_mac, &len);
    // dump(packet, len);
    ieee80211_mac_header* mac_header = (ieee80211_mac_header*)packet;

    //=========Make CSA frame=========


    //=========Parse beacon frame body=========
    beacon_frame_body* frame_body = (beacon_frame_body*)(packet + sizeof(ieee80211_mac_header));
    dump((const u_char*)frame_body, sizeof(*frame_body));

    return 0;
}