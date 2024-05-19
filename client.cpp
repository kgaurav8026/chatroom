#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
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

    char buffer[2048];  // Increased buffer size
    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = 0;
            cout << buffer << endl;
            cout.flush();  // Flush output stream
        }
        if (amountReceived == 0)
            break;
    }
    close(socketFD);
    pthread_exit(NULL);
    return NULL;
}

int main() {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(2000);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    int result = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (result < 0) {
        cerr << "Failed to connect to server" << endl;
        close(socketFD);
        return 1;
    }

    cout << "connection was successful" << endl;

    char name[256];
    cout << "Please enter your name: ";
    cin.getline(name, sizeof(name));

    send(socketFD, name, strlen(name), 0);

    char* line = NULL;
    size_t lineSize = 0;
    char buffer[2048];  // Increased buffer size

    cout << "Type and we will send (type 'exit' to quit)..." << endl;
    startListeningAndPrintMessagesOnNewThread(socketFD);

    while (true) {
        getline(&line, &lineSize, stdin);
        line[strlen(line) - 1] = 0;

        if (strcmp(line, "showmembers") == 0) {
            ssize_t bytesSent = send(socketFD, line, strlen(line), 0);
            if (bytesSent > 0) {
                ssize_t bytesReceived = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
                if (bytesReceived > 0) {
                    buffer[bytesReceived] = '\0';
                    cout << buffer << endl;
                    cout.flush();  // Flush output stream
                }
            }
        } else if (strlen(line) > 0) {
            sprintf(buffer, "%s : %s", name, line);
            if (strcmp(line, "exit") == 0)
                break;
            ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
        }
    }

    close(socketFD);
    free(line);

    return 0;
}