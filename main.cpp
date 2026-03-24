#include <iostream>
#include "pch.h"

void usage()
{
    std::cout << "Usage: ./csa <interface>" << std::endl;
}

int check_args(int argc, char* argv[])
{
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



int main(int argc, char* argv[]) {
    if (check_args(argc, argv) == EXIT_FAILURE) {
        return EXIT_FAILURE ;
    }

    // PCAP initialization
    char* dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    int dlt = pcap_datalink(pcap);
    std::cout << "DLT: " << dlt << std::endl;
    if (pcap == nullptr) 
    {
		fprintf(stderr, "couldn't open device %s(%s)\n", dev, errbuf);
		return EXIT_FAILURE;
	}



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