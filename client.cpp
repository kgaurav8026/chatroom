#include "socketutil.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

using namespace std;

void* _listenAndPrint(void* arg);
void startListeningAndPrintMessagesOnNewThread(int socketFD);

void startListeningAndPrintMessagesOnNewThread(int socketFD) {
    pthread_t id;
    int* socketFDPtr = new int(socketFD);
    pthread_create(&id, NULL, _listenAndPrint, socketFDPtr);
    pthread_detach(id);
}

void* _listenAndPrint(void* arg) {
    int socketFD = *(int*)arg;
    delete (int*)arg;

    char buffer[1024];
    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            cout << buffer << endl;
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
    struct sockaddr_in* address = createIPv4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr*)address, sizeof(struct sockaddr_in));

    if (result == 0)
        cout << "connection was successful" << endl;

    char* name = NULL;
    size_t nameSize = 0;
    cout << "please enter your name?" << endl;
    getline(&name, &nameSize, stdin);
    name[strlen(name) - 1] = 0;

    char* line = NULL;
    size_t lineSize = 0;
    cout << "type and we will send(type exit)..." << endl;

    startListeningAndPrintMessagesOnNewThread(socketFD);

    char buffer[1024];
    while (true) {
        getline(&line, &lineSize, stdin);
        line[strlen(line) - 1] = 0;
        sprintf(buffer, "%s : %s", name, line);
        if (strlen(line) > 0) {
            if (strcmp(line, "exit") == 0)
                break;
            ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
        }
    }

    free(address);
    close(socketFD);
    free(line);

    return 0;
}