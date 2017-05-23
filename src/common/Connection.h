#pragma once

#include "ProcessImage.h"

#include <string>
#include <netinet/in.h>

using std::string;

union sockaddr_union {
  sockaddr_in6 sin6;
  sockaddr_in sin;
};

std::pair<int, sockaddr_union> CreateSocket(const string& addr, int port);

class Connection {
    int socked_fd_;

public:
    Connection(int fd);
    Connection(const string& addr,int port);
    static std::pair<int, sockaddr_union> CreateSocket(const string& addr, int port);
    ~Connection();
    string RecvMsg();
    void SendMsg(const string &msg);

    void SendProcessImage(const ProcessImage&);
    ProcessImage RecvProcessImage(fs::path targetFileLocation);
};
