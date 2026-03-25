#include "handler.h"
#include "pch.h"
#include "radiotap.h"
#include "macframe.h"



pcap_t* handle_init(const char* dev, char* errbuf) {
    // Open the device for live capture
    pcap_t* pcap = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (pcap == nullptr) 
    {
        fprintf(stderr, "couldn't open device %s(%s)\n", dev, errbuf);
        return nullptr;
    }


    // Check the data link type
    int dlt = pcap_datalink(pcap);
    if (dlt != DLT_IEEE802_11_RADIO) {
        fprintf(stderr, "Not a radiotap interface (DLT=%d)\n", dlt);
        pcap_close(pcap);
        return nullptr;
    }

    std::cout << "DLT OK: " << dlt << std::endl;
    return pcap;
}

void dump(const u_char* buf, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (i != 0 && i % 16 == 0)
            printf("\n");
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

u_char* find_target_beacon(pcap_t* pcap, Config* config, int* len) {
    while (true) 
    {
        struct pcap_pkthdr* header;
        const u_char* packet;
        int res = pcap_next_ex(pcap, &header, &packet);
        if (res == 0)
        {
            continue; //time out
        } 
        else if(res <0)
        { // error
            printf("pcap_next_ex return %d(%s)\n", res, pcap_geterr(pcap));
            break;
        }

        ieee80211_radiotap_header* radiotap_header = (ieee80211_radiotap_header*) packet;
        // dump(packet, header->caplen);
        const u_char* ieee80211_header = packet + radiotap_header->it_len;
        ieee80211_mac_header* mac_header = (ieee80211_mac_header*) ieee80211_header;

        if(isBeaconFrame(mac_header) && isTargetAP(mac_header, config->ap_mac)) 
        {
            std::cout << "Found target AP's beacon frame!" << std::endl;
            *len = header->caplen;
            config->ap_current_channel = getCurrentChannel(packet);
            return (u_char*)ieee80211_header;
        }

    }
    //TODO : 특정 시간 동안 못찾으면 종료하는 로직 추가해야함.
    return nullptr;
}