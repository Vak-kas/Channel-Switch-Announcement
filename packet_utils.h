#pragma once
#include <vector>
#include <pcap.h>
#include <cstdint>
#include <cstring>
#include <algorithm>

struct InsertData {
    const uint8_t* pos;
    const uint8_t* data;
    size_t len;
};

u_char* inject_data_into_packet(const u_char* packet, int total_packet_len, 
    const std::vector<InsertData>& inserts, size_t& new_packet_len
);

void write_data_to_packet(u_char* packet, size_t offset, const uint8_t* data, size_t len);