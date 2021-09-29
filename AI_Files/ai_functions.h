#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>

#include "../json.hpp"
#include "../defines.h"
#include "../socket_defs.h"

using namespace std;
using json=nlohmann::json;

void updateBoard(char board[10][10], int row, int col, int length, Direction dir, char newChar);
int  socketConnect(int sock, const char *socket_name);
int  socketOpen(const char *socket_name);
void socketClose(int sock);
void sendGameVars(json &msg);
void messageHandler(json &msg, string &clientID);
void wipeBoards();

string clientID;


int main(int argc, char *argv[]){
    ssize_t waste;

    srand(getpid());
    clientID = to_string(getpid());

    //setup a socket and connection tools
    const char *path = "./serversocket";
    int clientSd = socketOpen(path);

    //create a message buffer
    char buffer[1500];


    while(1){
        //read
        memset(&buffer, 0, sizeof(buffer));//clear the buffer

        //recv(clientSd, (char*)&buffer, sizeof(buffer), 0);
        waste=read( clientSd , buffer, 1499);

        string tempStr="";
        tempStr.append(buffer);
        //strcpy(tempStr, buffer);

        json msg=json::parse(tempStr);

        messageHandler(msg, clientID);

        memset(&buffer, 0, sizeof(buffer));//clear the buffer

        if( msg.dump().length()+1 > 1500 ){
            cerr << "UNSUPPORTED LENGTH REACHED BY: msg" << endl;
        }
        strcpy(buffer, msg.dump().c_str());
        send(clientSd, (char*)&buffer, strlen(buffer), 0);
    }

    close(clientSd);
    cerr << "Connection closed for client " << clientID << endl;

    if(waste){
        //do nothing--this is just to stop a warning from popping up.
    }
    return 0;
}

void updateBoard(char board[10][10], int row, int col, int length, Direction dir, char newChar){
    if(dir==HORIZONTAL){
        for(int len=0;len<length;len++){
            board[row][col+len]=newChar;
        }
    }else if(dir==VERTICAL){
        for(int len=0;len<length;len++){
            board[row+len][col]=newChar;
        }
    }else if(dir==NONE){
        board[row][col]=newChar;
    }
}

int socketConnect(int sock, const char *socket_name){
    struct sockaddr_un address;

    memset(&address, 0x00, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_name, SOCKET_NAME_MAX_LEN-1);

    return connect(sock, (struct sockaddr *)&address, sizeof(address));
}

int socketOpen(const char *socket_name){
    int res;

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;

    res = socketConnect(sock, socket_name);
    if (res < 0) {
        close(sock);
        return res;
    }
    
    return sock;
}

void socketClose(int sock){
    if (sock > 0)
        close(sock);
}