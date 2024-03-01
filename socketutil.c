#include "socketutil.h"

struct sockaddr_in* createIPv4Address(char* ip, int port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(80); // convert machine byte order to  big endian format use by network
    inet_pton(AF_INET, ip,&address->sin_addr.s_addr); // func to convert string address to 32 bit int address
    return address;

}
int createTCPIpv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0); // AF_NET : ipv4, SOCK_STREAM : TCP, 0 : IP layer underneath this tcp layer
}