#pragma once

#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <iostream>




struct Config {
    std::string interface;
    std::string ap_mac;
    std::string ap_ssid;
};