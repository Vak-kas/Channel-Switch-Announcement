#include "macframe.h"

bool isBeaconFrame(const ieee80211_mac_header* header) 
{
    return header->getType() == FrameType::MANAGEMENT 
        && header->getSubType() == FrameMSubType::BEACON;
}

bool isTargetAP(const ieee80211_mac_header* header, const Mac& target_ap) 
{
    address_info info;
    setAddressInfo(header, &info);
    return info.BSSID == target_ap;
}

void setAddressInfo(const ieee80211_mac_header* header, address_info* info)
{
    uint8_t toDS = header->getToDS();
    uint8_t fromDS = header->getFromDS();

    if(!toDS && !fromDS) //Beacon 일때만 가정
    {
        info->Receiver = header->addr1;
        info->Transmitter = header->addr2;
        info->Destination = header->addr1;
        info->Source = header->addr2;
        info->BSSID = header->addr3;
    }
    else if(toDS && !fromDS)
    {
        info->Receiver = header->addr1;
        info->Transmitter = header->addr2;
        info->Destination = header->addr3;
        info->Source = header->addr2;
        info->BSSID = header->addr1;
    }
    else if(!toDS && fromDS)
    {
        info->Receiver = header->addr1;
        info->Transmitter = header->addr2;
        info->Destination = header->addr1;
        info->Source = header->addr3;
        info->BSSID = header->addr2;
    }
    else if(toDS && fromDS)
    {
        //TODO : 나중에 구현
    }
}
