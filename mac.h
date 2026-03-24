#pragma once
#include <cstdint>
#include <cstddef>
#include <stdio.h>
#include <string>
#include <cstring>
#include <cstdio>

struct Mac
{
    static constexpr size_t SIZE = 6;
private:
    uint8_t mac_[SIZE];

public:
    Mac();
    Mac(const uint8_t* other) {memcpy(this->mac_, other, SIZE);}
    Mac(const Mac& other) { memcpy(this->mac_, other.mac_, SIZE); }

    explicit operator std::string() const;

    Mac& operator=(const Mac& other) { memcpy(this->mac_, other.mac_, SIZE); return *this; }
    bool operator==(const Mac& other) const { return memcmp(this->mac_, other.mac_, SIZE) == 0; }
    bool operator!=(const Mac& other) const { return memcmp(this->mac_, other.mac_, SIZE) != 0; }

};