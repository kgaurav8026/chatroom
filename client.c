#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for inet_pton
#include <netinet/in.h>
#include <string.h>
#include<malloc.h>

int createTCPIpv4Socket();
struct sockaddr_in* createIPv4Address(char* ip , int port);


struct sockaddr_in* createIPv4Address(char* ip, int port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(80); // convert machine byte order to  big endian format use by network
    inet_pton(AF_INET, ip, &address->sin_addr.s_addr); // func to convert string address to 32 bit int address
    return address;

}
int createTCPIpv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0); // AF_NET : ipv4, SOCK_STREAM : TCP, 0 : IP layer underneath this tcp layer
}

int main() {
    int socketFD = createTCPIpv4Socket();  

    struct sockaddr_in* address = createIPv4Address("142.250.188.46",80);

    int result = connect(socketFD, address, sizeof *address);

    if (result == 0)
        printf("Connection was successful\n");
    else {
        perror("Connection failed");
        return 1;
    }

    char* message = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
    send(socketFD, message, strlen(message), 0);

    char buffer[1024];
    recv(socketFD, buffer, sizeof(buffer), 0);

    printf("Response was: %s\n", buffer);

    printf("\n\n");

    return 0;
}
