#include "connection.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <iostream> // TODO remove line

Connection::Connection(int fd):SocketFD(fd) {
}

Connection::Connection(const string& addr, int port) {
    // TODO: implement properly
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    res = inet_pton(AF_INET, addr.data(), &sa.sin_addr);

    if (res != 1) {
        std::cout << "wrong ip address" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connecting to: " << addr << std::endl;
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }


}

Connection::~Connection() {
    // TODO: implement properly
    if (shutdown(SocketFD, SHUT_RDWR) == -1) {
        perror("shutdown failed");
    }
    close(SocketFD);
}

string Connection::recvMsg() {
    // TODO: implement properly
    char buff[100];
    recv(SocketFD, buff, sizeof(buff), 0);
    return buff;
}

void Connection::sendMsg(const string& msg) {
    // TODO: implement properly
    send(SocketFD, msg.data(), strlen(msg.data()), 0);
}
