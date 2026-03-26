#include "management.h"
#include "pch.h"

void print_beacon_frame_body(const beacon_frame_body* body)
{
    std::cout << "Timestamp: " << body->timestamp << std::endl;
    std::cout << "Beacon Interval: " << body->beacon_interval << std::endl;
    std::cout << "Capability Info: " << std::hex << body->capability_info << std::dec << std::endl;
}