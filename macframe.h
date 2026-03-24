#pragma once
#include <stdlib.h>
#include "mac.h"

struct ieee80211_mac_header 
{
    uint16_t frame_control;
    uint16_t duration_id;
    Mac addr1;
    Mac addr2;
    Mac addr3;
    uint16_t seq_ctrl;

public:
    uint8_t getType() const { return (frame_control >> 2) & 0x3 ; }
    uint8_t getSubType() const { return (frame_control >> 4 ) & 0xF ; }

} __attribute__((__packed__));


bool isBeaconFrame(const ieee80211_mac_header* header) 
{
    return header->getType() == FrameType::MANAGEMENT 
        && header->getSubType() == FrameMSubType::BEACON;
}

enum FrameType 
{
    MANAGEMENT = 0,
    CONTROL = 1,
    DATA = 2,
};


enum FrameMSubType
{
    ASSOCIATION_REQUEST = 0,
    ASSOCIATION_RESPONSE = 1,
    REASSOCIATION_REQUEST = 2,
    REASSOCIATION_RESPONSE = 3,
    PROBE_REQUEST = 4,
    PROBE_RESPONSE = 5,
    BEACON = 8,
    ATIM = 9,
    DISASSOCIATION = 10,
    AUTHENTICATION = 11,
    DEAUTHENTICATION = 12,
};

enum FrameCSubType
{
    CONTROL_WRAPPER = 7,
};

enum FrameDSubType
{
    DATA = 0,
    DATA_CF_ACK = 1,
    DATA_CF_POLL = 2,
    DATA_CF_ACK_POLL = 3,
    NULL_DATA = 4,
    CF_ACK = 5,
    CF_POLL = 6,
    CF_ACK_POLL = 7,
};