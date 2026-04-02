#include "management.h"
#include "pch.h"


void print_beacon_frame_body(const beacon_frame_body* body)
{
    std::cout << "Timestamp: " << body->timestamp << std::endl;
    std::cout << "Beacon Interval: " << body->beacon_interval << std::endl;
    std::cout << "Capability Info: " << std::hex << body->capability_info << std::dec << std::endl;
}

void updateSN(u_char* packet)
{
    // Radiotap 헤더 접근
    ieee80211_radiotap_header* radiotap_header =
        (ieee80211_radiotap_header*)packet;

    size_t radiotap_len = radiotap_header->it_len;

    // MAC 헤더 접근
    ieee80211_mac_header* mac_header =
        (ieee80211_mac_header*)(packet + radiotap_len);

    // --- 1. 네트워크 바이트 → 호스트 바이트 ---
    uint16_t seq_ctrl = ntohs(mac_header->seq_ctrl);

    // --- 2. Sequence Number 추출 (상위 12비트) ---
    uint16_t seq_num = (seq_ctrl >> 4) & 0x0FFF;

    // --- 3. 증가 (0 ~ 4095 순환) ---
    seq_num = (seq_num + 1) % 4096;

    // --- 4. Fragment Number 유지 (하위 4비트) ---
    uint16_t frag_num = seq_ctrl & 0x000F;

    uint16_t new_seq_ctrl = (seq_num << 4) | frag_num;

    // --- 5. 다시 네트워크 바이트로 저장 ---
    mac_header->seq_ctrl = htons(new_seq_ctrl);
}