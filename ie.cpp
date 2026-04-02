#include "ie.h"
#include "pch.h"

channel_switch_announcement_ie create_channel_switch_announcement_ie(uint8_t new_channel_number, uint8_t channel_switch_count)
{
    channel_switch_announcement_ie ie;
    ie.element_id = ieee80211_IE::CHANNEL_SWITCH_ANNOUNCEMENT;
    ie.length = 3; // channel_switch_mode(1) + new_channel_number(1) + channel_switch_count(1)
    ie.channel_switch_mode = 1; // 1 : STA should stop transmission until switch, 0: STA can transmit until switch
    ie.new_channel_number = new_channel_number;
    ie.channel_switch_count = channel_switch_count;
    return ie;
}


const uint8_t* find_ie_end_by_tag(ieee80211_IE tag, const uint8_t* packet, int radiotap_len, int mac_header_len, int body_len, int total_packet_len)
{
    const uint8_t* ptr = packet + radiotap_len + mac_header_len + body_len;
    const uint8_t* end = packet + total_packet_len;

    while (ptr + 2 <= end) 
    {
        uint8_t current_tag = ptr[0];
        uint8_t len = ptr[1];

        if (ptr + 2 + len > end) break; 

        if (current_tag == tag) 
        {
            return ptr + 2 + len; // IE 끝 지점 반환
        }
        ptr += 2 + len;
    }
    // If tag not found, return end (append position)
    return end; // 해당 태그의 IE가 없는 경우 그냥 맨 뒤에 삽입
}