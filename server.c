#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10

// Colors for client names
const char *colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m"};

void *handleClient(void *arg) {
    int clientSocketFD = *(int *)arg;
    free(arg);

    // Receive client name
    char clientName[100];
    ssize_t nameLen = recv(clientSocketFD, clientName, sizeof(clientName) - 1, 0);
    if (nameLen > 0) {
        clientName[nameLen] = 0;
        printf("Client connected: %s%s\033[0m\n", colors[clientSocketFD % 5], clientName);
    }

    char buffer[1024];
    while (1) {
        ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer), 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            printf("%s%s: %s\033[0m", colors[clientSocketFD % 5], clientName, buffer);
        } else if (amountReceived == 0) {
            printf("Client disconnected: %s%s\033[0m\n", colors[clientSocketFD % 5], clientName);
            break;
        } else {
            perror("Error receiving data");
            break;
        }
    }

    close(clientSocketFD);
    return NULL;
}

int main() {
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(2000);

    int result = bind(serverSocketFD, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == -1) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocketFD, MAX_CLIENTS) == -1) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 2000...\n");

    while (1) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int *clientSocketFD = malloc(sizeof(int));
        *clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (*clientSocketFD == -1) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, clientSocketFD) != 0) {
            perror("Failed to create thread");
            close(*clientSocketFD);
            free(clientSocketFD);
        } else {
            pthread_detach(thread);
        }
    }

    close(serverSocketFD);

    return 0;
}
