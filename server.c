#include<stdbool.h>
#include "socketutil.h"


int main(){

    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);

    int result = bind(serverSocketFD, serverAddress,sizeof(*serverAddress));

    if(result == 0)
        printf("Server socket was bound successfully\n");
    int listenResult = listen(serverSocketFD, 10); // can queue up to 10 connections
    

    struct sockaddr_in clientAddress ;
    int clientAddressSize = sizeof (struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize);
    char buffer[1024];
    while(true){
         ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer), 0);  
         if(amountReceived > 0){
            buffer[amountReceived] = 0;
            printf("Response was: %s\n", buffer);

         }
        

        if(amountReceived == 0) break;
    }
    
    close(clientSocketFD);
    close(serverSocketFD, SHUT_RDWR);
   
    return 0;
}