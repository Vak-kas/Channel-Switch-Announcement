#pragma once
#include <pcap.h>

pcap_t* handle_init(const char* dev, char* errbuf);