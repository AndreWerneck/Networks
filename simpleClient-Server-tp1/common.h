//
// Created by Andre Werneck on 23/08/2020.
//

#ifndef TP1_COMMON_H
#define TP1_COMMON_H
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstring>
#include <string>
using namespace std;

void logexit(const char *message);

int buf_ascii2int(signed char c);

int onClientParseAddress(const char *addressStr,const char *portStr, struct sockaddr_storage *storage);

int onServerParseAddress(string protocol, const char *portStr,struct sockaddr_storage *storage);

void addressToString(struct sockaddr *address,char *str,size_t sizeOfStr);

#endif //TP1_COMMON_H
