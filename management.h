#pragma once
#include <stdlib.h>
#include <cstdint>
struct beacon_frame_body
{
    uint64_t timestamp;
    uint16_t beacon_interval; //default : 100ms (초당 10회 비콘 프레임 발송)
    uint16_t capability_info;
};