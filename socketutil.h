#ifndef SOCKETUTIL_SOCKETUTIL_H
#define SOCKETUTIL_SOCKETUTIL_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

struct sockaddr_in* createIPv4Address(const char* ip, int port);
int createTCPIpv4Socket();

#endif