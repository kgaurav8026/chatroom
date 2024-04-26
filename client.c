#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(2000);

    int result = connect(socketFD, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == -1) {
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    char clientName[100];
    printf("Enter your name: ");
    fgets(clientName, sizeof(clientName), stdin);
    clientName[strcspn(clientName, "\n")] = 0; // Remove newline character

    // Send client name to the server
    send(socketFD, clientName, strlen(clientName), 0);

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type a message (type 'exit' to quit):\n");

    while (1) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount == -1) {
            perror("Error reading input");
            break;
        }

        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        ssize_t amountSent = send(socketFD, line, charCount, 0);
        if (amountSent == -1) {
            perror("Error sending data");
            break;
        }
    }

    free(line);
    close(socketFD);

    return 0;
}