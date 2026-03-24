#include "handler.h"
#include "pch.h"


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