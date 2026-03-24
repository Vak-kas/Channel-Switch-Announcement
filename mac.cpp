#include "mac.h"
#include "pch.h"

Mac::operator std::string() const {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac_[0], mac_[1], mac_[2], mac_[3], mac_[4], mac_[5]);
    return std::string(buffer);
}