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