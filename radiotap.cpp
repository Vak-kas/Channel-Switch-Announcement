#include "radiotap.h"
#include <iostream>

void dump2(const u_char* buf, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (i != 0 && i % 16 == 0)
            printf("\n");
        printf("%02X ", buf[i]);
    }
    printf("\n");
}
int getCurrentChannel(const u_char* packet)
{
    ieee80211_radiotap_header* radiotap_header = (ieee80211_radiotap_header*) packet;
    dump2(packet, radiotap_header->it_len);
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