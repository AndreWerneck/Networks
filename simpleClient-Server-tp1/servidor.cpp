//
// Created by Andre Werneck on 22/08/2020.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include "common.h"
#include <string>
#include <zconf.h>

#define BUFSZ 256


using namespace std;

string VERSION("v6");
string PALAVRA("trabalho");


void usageServer(char **argv) {
    cout << "Usage:" << argv[0] << " <server port> " << endl;
    cout << "example: " << argv[0] << " 5151 " << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usageServer(argv);
    }
    int serverSocket;
    struct sockaddr_storage storage;
    int opt = 1; //enable setsockopt

    if (0 != onServerParseAddress(VERSION, argv[1], &storage)) { // parsing IPv family, port and address
        usageServer(argv);
    }

    serverSocket = socket(storage.ss_family, SOCK_STREAM, 0); // initializing the socket
    // SOCK_STREM -> TCP protocol and 0 -> IP protocol
    if (serverSocket == -1) {
        logexit("Socket Failed");
    }
    if (0 != setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        logexit("setsockopt failed");
    }

    struct sockaddr *address = (struct sockaddr *) (&storage); // address initialized with content of storage
    if (0 != bind(serverSocket, address, sizeof(storage))) { // binding
        logexit("bind failed");
    }
    if (0 != listen(serverSocket, 10)) {
        logexit("listen failed");
    }

    char addressStr[BUFSZ];
    addressToString(address, addressStr, BUFSZ);
    cout << "bound to " << addressStr << " waiting connections " << endl; // warns that server is bound

    // server accepts and waits for data transference
    while (true) {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddress = (struct sockaddr *) (&clientStorage);
        socklen_t clientAddressLen = sizeof(clientStorage);

        int newSocket = accept(serverSocket, clientAddress, &clientAddressLen);// newSocket talks with the client
        if (newSocket < 0) {
            logexit("accept failed");
        }
        char clientAddrStr[BUFSZ];
        addressToString(clientAddress, clientAddrStr, BUFSZ);
        cout << "connection from " << clientAddrStr << endl;

        char message[3]; // sends initial message with the type (1) and the word lenght
        memset(message, 0, 3); // initializes empty message
        sprintf(message, "%d%d", (uint8_t) 1,
                (uint8_t) PALAVRA.length()); //sending uint8_t ( 1 byte for each) to start the game
        send(newSocket, message, strlen(message), 0);

        int letterCount = 0;
        char buffer[2];
        while (letterCount < PALAVRA.length()) {
            memset(buffer, 0, 2);
            size_t count = recv(newSocket, buffer, 2, 0); // receive PALPITE from client
            if (buf_ascii2int(buffer[0]) == 2) {
                cout << "message received: " << buffer << " size:" << count << " from client: " << clientAddrStr<< endl;

                char letter = buffer[1];
                uint8_t aux = 0;
                string positions;
//                int index = 0;
                for (int i = 0; i < PALAVRA.length(); i++) {
                    // getting letter positions
                    if (PALAVRA[i] == letter) {
                        aux += 1; // repetitions of the letter
                        positions.append(to_string(i)); //positions
//                        index++;
                    }
                }

                letterCount += aux;
                char answerToPalpite[BUFSZ];
                memset(answerToPalpite, 0, BUFSZ);
                sprintf(answerToPalpite, "%d%d%s", (uint8_t) 3, aux, positions.c_str()); //sending answer from server
                send(newSocket, answerToPalpite, BUFSZ, 0);
                positions.clear();
                if (letterCount == PALAVRA.length()){
                    char endMsg[2];
                    memset(endMsg,0,2);
                    sprintf(endMsg, "%d",(uint8_t) 4);
                    send(newSocket, endMsg, strlen(endMsg), 0); // send answer to client
                    close(newSocket);
                }
            }
        }

    }

    exit(EXIT_SUCCESS);
}