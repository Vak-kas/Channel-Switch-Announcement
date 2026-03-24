#include "mac.h"
#include "pch.h"
#include "handler.h"


void usage()
{
    std::cout << "Usage: ./csa <interface> <ap_mac> " << std::endl;
}

int check_args(int argc, char* argv[])
{
    if (argc != 3) {
        usage();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



int main(int argc, char* argv[]) {
    if (check_args(argc, argv) == EXIT_FAILURE) {
        return EXIT_FAILURE ;
    }
    //==========Config initialization==========
    Config config;
    config.interface = argv[1];
    config.ap_mac = Mac(argv[2]);

    //==========PCAP initialization==========
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = handle_init(config.interface.c_str(), errbuf);
    if(pcap == nullptr) {
        return EXIT_FAILURE;
    }
    //=======================================

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
        else
        {
            printf("패킷 길이: %d\n", header->caplen);
        }
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}