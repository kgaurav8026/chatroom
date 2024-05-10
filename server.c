#include "socketutil.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

struct AcceptedSocket {
    int acceptedSocketFD;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);
void freeAcceptedSocket(struct AcceptedSocket *pSocket);
static void *receiveAndPrintIncomingDataWrapper(void *arg);
void receiveAndPrintIncomingData(int socketFD);
void sendReceivedMessageToTheOtherClients(char *buffer, int senderSocketFD);

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;

    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

void startAcceptingIncomingConnections(int serverSocketFD) {
    while (true) {
        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);

        if (clientSocket->acceptedSuccessfully) {
            acceptedSockets[acceptedSocketCount++] = *clientSocket;
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
            freeAcceptedSocket(clientSocket);
        } else {
            // Handle error condition
            freeAcceptedSocket(clientSocket);
        }

        // Add a condition to break out of the loop if needed
        // For example, you could check for a specific signal or user input
    }
}

static void *receiveAndPrintIncomingDataWrapper(void *arg) {
    int *socketFD = (int *)arg;
    receiveAndPrintIncomingData(*socketFD);
    free(arg);
    pthread_exit(NULL);
    return NULL;
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket) {
    pthread_t id;
    int *socketFD = malloc(sizeof(int));
    *socketFD = pSocket->acceptedSocketFD;
    pthread_create(&id, NULL, receiveAndPrintIncomingDataWrapper, socketFD);
    pthread_detach(id);
}

void receiveAndPrintIncomingData(int socketFD) {
    char buffer[1024];
    while (1) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            printf("%s\n", buffer);
            sendReceivedMessageToTheOtherClients(buffer, socketFD);
        }

        if (amountReceived == 0)
            break;
    }

    close(socketFD);
}

void sendReceivedMessageToTheOtherClients(char *buffer, int senderSocketFD) {
    for (int i = 0; i < acceptedSocketCount; i++) {
        int recipientSocketFD = acceptedSockets[i].acceptedSocketFD;
        if (recipientSocketFD != senderSocketFD) {
            send(recipientSocketFD, buffer, strlen(buffer), 0);
        }
    }
}

void freeAcceptedSocket(struct AcceptedSocket *pSocket) {
    free(pSocket);
}

int main() {
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);
    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));

    if (result == 0) {
        printf("Socket was bound successfully\n");
    }

    int listenResult = listen(serverSocketFD, 10);

    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}
