//
// Created by Andre Werneck on 23/08/2020.
//

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstring>
#include "common.h"
#include <iostream>


using namespace std;

void logexit(const char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

int buf_ascii2int(signed char c){
    int i = (int) (unsigned char)c - 48;
    return i;
}

int onClientParseAddress(const char *addressStr,const char *portStr, struct sockaddr_storage *storage){
    if (addressStr == nullptr || portStr == nullptr){
        return -1;
    }
    uint16_t port = (uint16_t )atoi(portStr); // 16 bits is the standard for the port
    if (port == 0){
        return -1;
    }
    port = htons(port);// converting for the right endianness

    // implementation for IPV4
    struct in_addr ipv4Addr; // 32 bits
    if (inet_pton(AF_INET,addressStr,&ipv4Addr)){ // converting address from string to byte format
        struct sockaddr_in *address4 = (struct sockaddr_in *)storage;
        address4->sin_family = AF_INET;
        address4->sin_port = port;
        address4->sin_addr = ipv4Addr;
        return 0;
    }
    // implementation for IPV6
    struct in6_addr ipv6Addr; // 128 bits
    if (inet_pton(AF_INET6,addressStr,&ipv6Addr)){
        struct sockaddr_in6 *address6 = (struct sockaddr_in6 *)storage;
        address6->sin6_family  = AF_INET6;
        address6->sin6_port = port;
        memcpy(&(address6->sin6_addr),&ipv6Addr,sizeof(ipv6Addr));
        return 0;
    }
    return -1;
}

int onServerParseAddress(string protocol, const char *portStr,struct sockaddr_storage *storage){
    uint16_t port = (uint16_t )atoi(portStr); // 16 bits is the standard for the port
    if (port == 0){
        return -1;
    }
    port = htons(port);// converting for the right endianness

    memset(storage,0,sizeof(*storage));
    if (0 == protocol.compare("v4")){
        struct sockaddr_in *address4 = (struct sockaddr_in *)storage;
        address4->sin_family = AF_INET;
        address4->sin_port = port;
        address4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }else if( 0 == protocol.compare("v6")){
        struct sockaddr_in6 *address6 = (struct sockaddr_in6 *)storage;
        address6->sin6_family = AF_INET6;
        address6->sin6_port = port;
        address6->sin6_addr = in6addr_any;
        return 0;
    }else{
        return -1;
    }
}

void addressToString(struct sockaddr *address,char *str,size_t sizeOfStr){
    int version;
    uint16_t port;
    char addressStr[INET6_ADDRSTRLEN+1] = "";

    if (address->sa_family == AF_INET){
        version = 4;
        struct sockaddr_in *address4 = (struct sockaddr_in *)address;

        if(!inet_ntop(AF_INET,&(address4->sin_addr),addressStr,INET6_ADDRSTRLEN+1)){ // converting from network to presentation
            cout << " ntop failed " << endl;
            exit(EXIT_FAILURE);
        }
        port = ntohs(address4->sin_port); // doing the inverse from htons for ipv4

    } else if(address->sa_family == AF_INET6){
        version = 6;
        struct sockaddr_in6 *address6 = (struct sockaddr_in6 *)address;
        if(!inet_ntop(AF_INET6,&(address6->sin6_addr),addressStr,INET6_ADDRSTRLEN+1)){
            cout << " ntop failed " << endl;
            exit(EXIT_FAILURE);
        }
        port = ntohs(address6->sin6_port); // doing the same for ipv6
    } else{
        cout << " family not found " << endl;
        exit(EXIT_FAILURE);
    }
    if(str) {
        snprintf(str, sizeOfStr, "IPv%d %s %hu", version, addressStr, port);
    }
}