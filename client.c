#include <stdbool.h>
#include "socketutil.h"


int main() {
    int socketFD = createTCPIpv4Socket();  

    struct sockaddr_in* address = createIPv4Address("127.0.0.0",2000);

    int result = connect(socketFD, (struct sockaddr*)address, sizeof(struct sockaddr_in));

    if (result == 0)
        printf("Connection was successful\n");
    else {
        perror("Connection failed");
        return 1;
    }

    char *line = NULL;
    size_t lineSize = 0;

   
    printf("type and we will send(type exit)..\n");
    while(true){
        ssize_t charCount = getline(&line, &lineSize, stdin);
       
        if(charCount>0){
            if(strcmp(line,"exit\n") == 0)
                break;
             ssize_t amountWasSend = send(socketFD, line, charCount, 0);

        }

    }
    close(socketFD);
    return 0;
}
