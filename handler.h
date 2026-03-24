#pragma once
#include <pcap.h>
#include "mac.h"

pcap_t* handle_init(const char* dev, char* errbuf);
u_char* find_target_beacon(pcap_t* pcap, Mac ap_mac, int* len);
void dump(const u_char* buf, int size);