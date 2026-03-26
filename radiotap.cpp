#include "radiotap.h"
#include <iostream>

//채널 가져오는 함수 (현재는 버그로 인해 채널 고정)
int getCurrentChannel(const u_char* packet)
{
    ieee80211_radiotap_header* radiotap_header = (ieee80211_radiotap_header*) packet;
    uint32_t present = radiotap_header->it_present;
    const uint8_t* ptr = packet + sizeof(*radiotap_header);

    // TSFT
    if (present & (1 << IEEE80211_RADIOTAP_TSFT))
        ptr += 8;

    // FLAGS
    if (present & (1 << IEEE80211_RADIOTAP_FLAGS))
        ptr += 1;

    // RATE
    if (present & (1 << IEEE80211_RADIOTAP_RATE))
        ptr += 1;

    // CHANNEL
    if (present & (1 << IEEE80211_RADIOTAP_CHANNEL)) {
        uint16_t freq = *(uint16_t*)ptr;

        std::cout << freq << std::endl;

        int channel = (freq - 2407) / 5;
        return channel;
    }

    return -1;

}

bool isFCS(const u_char* packet)
{
    ieee80211_radiotap_header* radiotap_header = (ieee80211_radiotap_header*) packet;
    uint32_t present = radiotap_header->it_present;
    const uint8_t* ptr = packet + sizeof(*radiotap_header);

    if (!(present & (1 << IEEE80211_RADIOTAP_FLAGS)))
        return false;

    uint8_t flags = *ptr;

    // FCS bit 확인
    return (flags & IEEE80211_RADIOTAP_F_FCS);
}