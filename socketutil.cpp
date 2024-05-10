#include "socketutil.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

using namespace std;

struct sockaddr_in* createIPv4Address(const char* ip, int port) {
    struct sockaddr_in* addr = new sockaddr_in();
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(addr->sin_addr));
    return addr;
}

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}