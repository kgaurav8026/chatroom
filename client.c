#include "socketutil.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

void *listenAndPrint(void *arg);
void startListeningAndPrintMessagesOnNewThread(int socketFD);

void startListeningAndPrintMessagesOnNewThread(int socketFD) {
    pthread_t id;
    int *socketFDPtr = malloc(sizeof(int));
    *socketFDPtr = socketFD;
    pthread_create(&id, NULL, listenAndPrint, socketFDPtr);
    pthread_detach(id);
}

void *listenAndPrint(void *arg) {
    int socketFD = *(int *)arg;
    free(arg);

    char buffer[1024];
    while (1) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            printf("%s\n", buffer);
        }

        if (amountReceived == 0)
            break;
    }

    close(socketFD);
    pthread_exit(NULL);
    return NULL;
}

int main() {
    int socketFD = createTCPIpv4Socket();
    struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr *)address, sizeof(struct sockaddr_in));

    if (result == 0)
        printf("connection was successful\n");


    char *name = NULL;
    size_t nameSize = 0;
    printf("please enter your name?\n");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount-1] = 0;


    char *line = NULL;
    size_t lineSize = 0;
    printf("type and we will send(type exit)...\n");

    startListeningAndPrintMessagesOnNewThread(socketFD);

    char buffer[1024];

    while (true) {

        
        ssize_t charCount = getline(&line, &lineSize, stdin);
        line[charCount-1] = 0;
        sprintf(buffer,"%s : %s", name, line);

        if (charCount > 0) {
            if (strcmp(line, "exit\n") == 0)
                break;

            ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
        }
    }

    free(address);
    close(socketFD);
    free(line);

    return 0;
}
