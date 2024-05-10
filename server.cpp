#include "socketutil.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

struct AcceptedSocket {
    int acceptedSocketFD;
    int error;
    bool acceptedSuccessfully;
};

AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;

void receiveAndPrintIncomingDataOnSeparateThread(AcceptedSocket* pSocket);
void freeAcceptedSocket(AcceptedSocket* pSocket);
static void* _receiveAndPrintIncomingDataWrapper(void* arg);
void receiveAndPrintIncomingData(int socketFD);
void sendReceivedMessageToTheOtherClients(char* buffer, int senderSocketFD);

AcceptedSocket* acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);

    AcceptedSocket* acceptedSocket = new AcceptedSocket();
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;
    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

void startAcceptingIncomingConnections(int serverSocketFD) {
    while (true) {
        AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
        if (clientSocket->acceptedSuccessfully) {
            acceptedSockets[acceptedSocketCount++] = *clientSocket;
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
            freeAcceptedSocket(clientSocket);
        }
        else {
            // Handle error condition
            freeAcceptedSocket(clientSocket);
        }
        // Add a condition to break out of the loop if needed
        // For example, you could check for a specific signal or user input
    }
}

static void* _receiveAndPrintIncomingDataWrapper(void* arg) {
    int* socketFD = (int*)arg;
    receiveAndPrintIncomingData(*socketFD);
    delete socketFD;
    pthread_exit(NULL);
    return NULL;
}

void receiveAndPrintIncomingDataOnSeparateThread(AcceptedSocket* pSocket) {
    pthread_t id;
    int* socketFD = new int(pSocket->acceptedSocketFD);
    pthread_create(&id, NULL, _receiveAndPrintIncomingDataWrapper, socketFD);
    pthread_detach(id);
}

void receiveAndPrintIncomingData(int socketFD) {
    char buffer[1024];
    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            cout << buffer << endl;
            sendReceivedMessageToTheOtherClients(buffer, socketFD);
        }
        if (amountReceived == 0)
            break;
    }
    close(socketFD);
}

void sendReceivedMessageToTheOtherClients(char* buffer, int senderSocketFD) {
    for (int i = 0; i < acceptedSocketCount; i++) {
        int recipientSocketFD = acceptedSockets[i].acceptedSocketFD;
        if (recipientSocketFD != senderSocketFD) {
            send(recipientSocketFD, buffer, strlen(buffer), 0);
        }
    }
}

void freeAcceptedSocket(AcceptedSocket* pSocket) {
    delete pSocket;
}

int main() {
    int serverSocketFD = createTCPIpv4Socket();
    char ip[] = ""; 
    struct sockaddr_in* serverAddress = createIPv4Address(ip, 2000);
    int result = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(struct sockaddr_in));

    if (result == 0) {
        cout << "Socket was bound successfully" << endl;
    }

    int listenResult = listen(serverSocketFD, 10);
    startAcceptingIncomingConnections(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}
