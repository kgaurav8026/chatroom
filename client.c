#include "socketutil.h"

int main() {
    int socketFD = createTCPIpv4Socket();  

    struct sockaddr_in* address = createIPv4Address("142.250.188.46",80);

    int result = connect(socketFD, (struct sockaddr*)address, sizeof(struct sockaddr_in));



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
