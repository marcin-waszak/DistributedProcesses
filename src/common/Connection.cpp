#include "Connection.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <iostream> // TODO remove line
#include <fstream>

static int Resolve(const string &address, addrinfo** info) {
    int result = getaddrinfo(address.c_str(), NULL, NULL, info);
    if (result) {
        std::cout << "Invalid address" << std::endl
            << gai_strerror(result) << std::endl;
        return -1;
    }
    return 0;
}

std::pair<int, sockaddr_union> Connection::CreateSocket(const string& addr, int port) {
    addrinfo* info = NULL;
    if(Resolve(addr, &info) < 0) {
        perror("Cannot resolve()");
        return std::make_pair(-1, sockaddr_union());
    }

    std::cout << "Resolve success" << std::endl;

    int address_format = info->ai_family;
    int packet_format = address_format == AF_INET6 ? PF_INET6 : PF_INET;
    int socket_fd = socket(packet_format, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket()");
        return std::make_pair(-1, sockaddr_union());
    }

    int enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    sockaddr_union sa;
    memset(&sa, 0, sizeof(sa));
    sockaddr_in* addr_info;   // TODO: try to use sockaddr_union
    sockaddr_in6* addr_info6; //

    switch(address_format) {
        case AF_INET:
            std::cout << "Using AF_INET\n";
            addr_info = (sockaddr_in*)info->ai_addr;
            sa.sin.sin_family = AF_INET;
            sa.sin.sin_port = htons(port);
            sa.sin.sin_addr = addr_info->sin_addr;
            break;

        case AF_INET6:
            std::cout << "Using AF_INET6\n";
            addr_info6 = (sockaddr_in6*)info->ai_addr;
            sa.sin6.sin6_family = AF_INET6;
            sa.sin6.sin6_port = htons(port);
            sa.sin6.sin6_addr = addr_info6->sin6_addr;
            break;

      default:
            return std::make_pair(-1, sockaddr_union());
    }

    //  freeaddrinfo(info);

    return std::make_pair(socket_fd, sa);
}

Connection::Connection(int fd):socked_fd_(fd), valid_(true) {
}

bool Connection::Connect() {
    auto soc = CreateSocket(addr_, port_);
    socked_fd_ = soc.first;
    if (socked_fd_ == -1) {
        valid_ = false;
        return false;
    }
    sockaddr_union sa = soc.second;
    std::cout << "Connecting to: " << addr_ << std::endl;
    if (connect(socked_fd_, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");

        if(close(socked_fd_) == -1)
            perror("close failed");
        valid_ = false;
        return false;
    }
    valid_ = true;
    return true;
}

bool Connection::Valid()const {
    return valid_;
}

Connection::Connection(const string& addr, int port)
        :addr_(addr), port_(port), valid_(false) {
    Connect();
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
    if (!valid_) throw std::logic_error("Trying to use invalid connection.");
    // TODO: implement properly: error handling, buffer overflow
    int size;
    recv(socked_fd_, &size, sizeof(unsigned), 0);
    string ret(size, 0);
    recv(socked_fd_, &ret[0], size, 0);
    return ret;
}

void Connection::SendMsg(const string &msg) {
    if (!valid_) throw std::logic_error("Trying to use invalid connection.");
    // TODO: error handling
    unsigned size = strlen(msg.data());
    send(socked_fd_, &size, sizeof(unsigned), 0);
    send(socked_fd_, msg.data(), strlen(msg.data()), 0);
}

void Connection::SendProcessImage(const ProcessImage& pi) {
    if (!valid_) throw std::logic_error("Trying to use invalid connection.");
    // TODO: implement properly: error handling
    std::vector<char> data = pi.GetBytes();
    send(socked_fd_, data.data(), data.size() * sizeof(char), 0);
}

ProcessImage Connection::RecvProcessImage(fs::path targetFileLocation) {
    if (!valid_) throw std::logic_error("Trying to use invalid connection.");
    // TODO: error handling
    // copied from stack overflow
    std::ofstream file;
    file.open(targetFileLocation.string(), std::ios::out | std::ios::binary);
    assert(file.is_open());
    char buffer[255];
    while (1) {
        ssize_t p = read(socked_fd_, buffer, sizeof(buffer));
        assert(p != -1);
        if (p == 0)
            break;
        file.write(buffer, p);
    }
    file.close();

    return ProcessImage(targetFileLocation);
}
