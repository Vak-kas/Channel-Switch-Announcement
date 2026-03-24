#include <stdlib.h>
struct beacon_frame_body
{
    uint64_t timestamp;
    uint16_t beacon_interval; //default : 100ms
    uint16_t capability_info;
    char* ssid;
};