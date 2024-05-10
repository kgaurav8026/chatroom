#include "socketutil.h"

int createTCPIpv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0); // AF_NET : ipv4, SOCK_STREAM : TCP, 0 : IP layer underneath this tcp layer
}

struct sockaddr_in* createIPv4Address(char* ip, int port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port); // convert machine byte order to  big endian format use by network
    if(strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY;  //represents an IP address that is used when we don’t want to bind a socket to any specific IP and accept connection from all  available interfaces
    else
        inet_pton(AF_INET, ip,&address->sin_addr.s_addr); // func to convert string address to 32 bit int address
    return address;

}