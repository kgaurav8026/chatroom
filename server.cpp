#include "socketutil.h"

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct AcceptedSocket {
    int acceptedSocketFD;
    int error;
    bool acceptedSuccessfully;
};

AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;

struct ClientInfo {
    int socketFD;
    string name;
};

vector<ClientInfo> activeClients;

void receiveAndPrintIncomingDataOnSeparateThread(AcceptedSocket* pSocket);
void freeAcceptedSocket(AcceptedSocket* pSocket);
static void* _receiveAndPrintIncomingDataWrapper(void* arg);
void receiveAndPrintIncomingData(int socketFD);
void sendReceivedMessageToTheOtherClients(char* buffer, int senderSocketFD);
void removeClientFromActiveClients(int socketFD);

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
    else {
        // Prompt the new client for their name
        char name[256];
        ssize_t bytesReceived = recv(clientSocketFD, name, sizeof(name) - 1, 0);
        if (bytesReceived > 0) {
            name[bytesReceived] = '\0';
            ClientInfo newClient = { clientSocketFD, string(name) };
            activeClients.push_back(newClient);
        }
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
        if (amountReceived == 0) {
            removeClientFromActiveClients(socketFD);
            break;
        }
    }
    close(socketFD);
}

void sendReceivedMessageToTheOtherClients(char* buffer, int senderSocketFD) {
    string message(buffer);
    if (message == "showmembers") {
        string memberList = "Active members:\n";
        for (const auto& client : activeClients) {
            memberList += client.name + "\n";
        }
        send(senderSocketFD, memberList.c_str(), memberList.length(), 0);
    } else if (message.find("exit") != string::npos) {
        removeClientFromActiveClients(senderSocketFD);
    } else {
        for (int i = 0; i < acceptedSocketCount; i++) {
            int recipientSocketFD = acceptedSockets[i].acceptedSocketFD;
            if (recipientSocketFD != senderSocketFD) {
                send(recipientSocketFD, buffer, strlen(buffer), 0);
            }
        }
    }
}

void removeClientFromActiveClients(int socketFD) {
    auto it = find_if(activeClients.begin(), activeClients.end(), [&](const ClientInfo& client) {
        return client.socketFD == socketFD;
    });

    if (it != activeClients.end()) {
        activeClients.erase(it);
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