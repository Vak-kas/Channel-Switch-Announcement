#pragma once
#include <stdlib.h>
#include <cstdint>

struct channel_switch_announcement_ie
{
    uint8_t element_id;
    uint8_t length;
    uint8_t channel_switch_mode;
    uint8_t new_channel_number;
    uint8_t channel_switch_count;
} __attribute__((__packed__));

struct ie_injector
{
    
};


channel_switch_announcement_ie create_channel_switch_announcement_ie(uint8_t new_channel_number, uint8_t channel_switch_count);
const uint8_t* find_ie_end_by_tag(ieee80211_IE tag, const uint8_t* packet, int radiotap_len, int mac_header_len, int body_len, int total_packet_len); 

enum ieee80211_IE 
{
    SSID = 0,
    SUPPORTED_RATES = 1,
    DSSS_PARAMETER_SET = 3,
    TIM = 5,
    COUNTRY = 7,
    POWER_CONSTRAINT = 32,
    SUPPORTED_CHANNELS = 36,
    CHANNEL_SWITCH_ANNOUNCEMENT = 37,
    QUIET  = 40,
    VENDOR_SPECIFIC = 221
}; 
//Reference : https://www.nsnam.org/docs/release/3.28/doxygen/wifi-information-element_8h_source.html


