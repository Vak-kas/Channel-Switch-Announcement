#pragma once
#include <pcap.h>
#include "mac.h"
#include "config.h"
#include "radiotap.h"

pcap_t* handle_init(const char* dev, char* errbuf);
u_char* find_target_beacon(pcap_t* pcap, Config* config, int* len, ieee80211_radiotap_header** rt_hdr);
void dump(const u_char* buf, int size);
int setSwitchChannel(int current_channel);