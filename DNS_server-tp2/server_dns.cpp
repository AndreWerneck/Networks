//
// Created by Andre Werneck on 27/09/2020.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include "util.h"
#include <fstream>
#include <pthread.h>
#include <list>


#define BUFSZ 256

using namespace std;

string VERSION("v6");

struct Host{
    string hostname;
    string ip;
};

struct server_data{
    const char *port;
//    string ip;
};


void usageServer(char **argv) {
    cout << "Usage:" << argv[0] << " <server port> " << endl;
    cout << "example: " << argv[0] << " 5151 " << endl;
    exit(EXIT_FAILURE);
}

void add(const string& hnameANDip,ofstream& file, string fileName){

    // writing input on opened file
    if(file.is_open()) {
        file << hnameANDip.substr(4,hnameANDip.length()) << endl;
    }

    cout<<"Success: "<< hnameANDip.substr(4,hnameANDip.length()) << " wrote successfully to file "<< fileName <<endl;

}

string search(Host host,string fileName, string inputToSearch,char **argv){
    ifstream file(fileName);
    string searchHost = inputToSearch.substr(7,inputToSearch.length());
    if (file.is_open()){
        while(file >> host.hostname >> host.ip){
            if ((host.hostname) == searchHost) {
                cout << "Host found on IP: " << host.ip << endl;
                return host.ip;
            } else {
                cout << "Host not found on port " << argv[1] << endl;
                return ("Not Found");
            }
        }
    }
}

void *server_thread(void *data){
    struct server_data * sdata = (struct server_data *)data;
    int serverSocket;
    struct sockaddr_storage storage;

    if (0 != onServerParseAddress(VERSION, sdata->port, &storage)) { // parsing IPv family, port and address
//        usageServer(argv);
        perror("socket creation failed");
    }

    serverSocket = socket(storage.ss_family, SOCK_DGRAM, 0); // initializing the socket
    // SOCK_DGRAM -> UDP protocol and 0 -> use default protocol for the address family.
    if (serverSocket == -1) {
        logexit("Socket Failed");
    }

    struct sockaddr *address = (struct sockaddr *) (&storage); // address initialized with content of storage
    if (0 != bind(serverSocket, address, sizeof(storage))) { // binding
        logexit("bind failed");
    } else{
        char addressStr[BUFSZ];
        addressToString(address, addressStr, BUFSZ);
        cout << "bound to " << addressStr << " waiting connections " << endl; // warns that server is bound
    }

}

list<string> link(string inputToLink,list<string> ls){
    string delimeter = " ";
    // parsing input
    string serverToLink = inputToLink.substr(5,inputToLink.length());
//    const char* ip = serverToLink.substr(0,serverToLink.find(delimeter)).c_str();
//    serverToLink.erase(0,serverToLink.find(delimeter)+delimeter.length());
//    const char* port = serverToLink.c_str();

    ls.push_back(serverToLink);
    return ls;
}



int main(int argc, char **argv) {
    if (argc != 2) {
        usageServer(argv);
    }
    pthread_t tid; // declaring thread

    // initializing data for thread
    struct server_data *sData = new server_data;
    if(!sData){
        logexit("Malloc failed");
    }

    sData->port = argv[1];


    // creating thread for socket with other servers
    if(pthread_create(&tid, nullptr,server_thread,sData) < 0){
        logexit("Could not create thread");
    }

    string name(argv[1]);
    name+=".txt";
    // declaring file to function add
    ofstream file(name);

    while(true){
        // displaying user's interface
        cout<< "Options are: "<<endl;
        cout<< "[1] add <hostname> <ip>"<<endl;
        cout<< "[2] search <hostname>"<<endl;
        cout<< "[3] link <ip> <port>"<<endl;

        // receiving input from user
        string input;
        Host h;
        list<string> serversToLink;
        string searchedIp;

        getline(cin,input);

//        cout<<input<<endl;

        if(input.compare(0,3,"add")==0){
            // call add function
            add(input,file,name);
        }
        cout<<endl;

        if(input.compare(0,6,"search")==0){
//            cout<<"search"<<endl;
            // call search function
            searchedIp = search(h,name,input,argv);
        }
        cout<<endl;

        if(input.compare(0,4,"link")==0){
            //call link function
            serversToLink = link(input,serversToLink);
            showlist(serversToLink);
        }

    }

    // server accepts and waits for data transference
return 0;
}