//
// Created by Andre Werneck on 22/08/2020.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <zconf.h>
#include "common.h"

#define BUFSZ 256

using namespace std;

struct Message {
    uint8_t type; // 8 - bits for the type
    uint8_t size; // 8 - bits for the size of the word
    char character; // 8 - bits for the character
};

void usageClient(char **argv) {
    cout << "Usage:" << argv[0] << " <server IP> <server port> " << endl;
    cout << "example: " << argv[0] << " ::1 5151 " << endl;
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if (argc != 3) {
        usageClient(argv);
    }

    int clientSocket;
    struct sockaddr_storage storage; //storages the server address

    if (0 != onClientParseAddress(argv[1], argv[2], &storage)) { // parsing argvs
        usageClient(argv);
    }

    clientSocket = socket(storage.ss_family, SOCK_STREAM, 0); //starting the socket
    if (clientSocket == -1) {
        logexit("socket failed");
    }

    struct sockaddr *address = (struct sockaddr *) (&storage); // instantiating the address

    // CONNECTING
    if (0 != connect(clientSocket, address, sizeof(storage))) {
        logexit("connection failed");
    }
    char addressStr[BUFSZ];
    addressToString(address, addressStr, BUFSZ);
    cout << "connected to " << addressStr << endl; // connected

    char word[2];
    struct Message init;

    memset(word, 0, 2);
    int first = recv(clientSocket, word, 2, 0); // recv the size of the word and init the game
    init.type = (uint8_t) buf_ascii2int(word[0]); // converting from the ascii table
    init.size = (uint8_t) buf_ascii2int(word[1]);

    if (init.type == 1) {
        int letterCount = 0;
        printf("Type = %d and Size = %d\n", init.type, init.size);
        cout << "Game Started" << endl;

        while (true) {
            // palpite
            char answerFromPalpite[BUFSZ];

            if (letterCount==init.size){ // letters are already discovered
                memset(answerFromPalpite, 0, BUFSZ);

                // waiting END GAME message from server
                int recvAnswer = recv(clientSocket, answerFromPalpite, BUFSZ,0);
                // verifying if message type is 4
                if (buf_ascii2int(answerFromPalpite[0]) == 4) {
                    // if it is 4 -> ends the game and closes the connection
                    close(clientSocket);
                    logexit("Word was discovered and the game has ended!");
                }
            }
            struct Message palpite;
            char letter[1];
            memset(letter, 0, 1);
            cout << "try letter = ";
            cin >> letter;
            palpite.type = 2;
            palpite.character = letter[0];
            char msg[3];
            memset(msg, 0, 3);
            sprintf(msg, "%d%c", palpite.type, palpite.character);
            send(clientSocket, msg, 3, 0); // send the "palpite"

            memset(answerFromPalpite, 0, BUFSZ);

            // receiving the answer from the palpite from server
            int recvAnswer = recv(clientSocket, answerFromPalpite, BUFSZ,0);

            if (buf_ascii2int(answerFromPalpite[0]) == 3) {
                cout << "Answer from server= " << answerFromPalpite << endl;
                if(buf_ascii2int(answerFromPalpite[1])>0){
                    letterCount+=buf_ascii2int(answerFromPalpite[1]);
                }
            }
        }
    }

    exit(EXIT_SUCCESS);
}
