#pragma once
#include <string>
#include "mac.h"

#define CSA_COUNT 5

struct Config {
    std::string interface;
    Mac ap_mac;
    int ap_current_channel;
};

extern Config config;