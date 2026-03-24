#pragma once

#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include "mac.h"




struct Config {
    std::string interface;
    Mac ap_mac;
};

extern Config config;