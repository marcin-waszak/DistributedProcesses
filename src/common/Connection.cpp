#include "Connection.h"

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

Connection::Connection(int fd):socked_fd_(fd) {
}

Connection::Connection(const string& addr, int port) {
    // TODO: implement properly
    socked_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socked_fd_ == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa_, 0, sizeof sa_);

    sa_.sin_family = AF_INET;
    sa_.sin_port = htons(port);
    int result = inet_pton(AF_INET, addr.data(), &sa_.sin_addr);

    if (result != 1) {
        std::cout << "wrong ip address" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connecting to: " << addr << std::endl;
    if (connect(socked_fd_, (struct sockaddr *)&sa_, sizeof sa_) == -1) {
        perror("connect failed");

        if(close(socked_fd_) == -1)
            perror("close failed");

        exit(EXIT_FAILURE);
    }
}

Connection::~Connection() {
    // TODO: implement properly
    if (shutdown(socked_fd_, SHUT_RDWR) == -1) {
        perror("shutdown failed");
    }

    if(close(socked_fd_) == -1)
        perror("close failed");
}

string Connection::RecvMsg() {
    // TODO: implement properly
    char buff[100];
    recv(socked_fd_, buff, sizeof(buff), 0);
    return buff;
}

void Connection::SendMsg(const string &msg) {
    // TODO: implement properly
    send(socked_fd_, msg.data(), strlen(msg.data()), 0);
}
