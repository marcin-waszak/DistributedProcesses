#pragma once

#include "ProcessImage.h"

#include <string>
#include <netinet/in.h>

using std::string;

union sockaddr_union {
  sockaddr_in6 sin6;
  sockaddr_in sin;
};

class Connection {
    int socked_fd_;

    // for reconnection
    string addr_;
    int port_;

    bool valid_;

public:
    Connection(int fd);
    Connection(const string& addr,int port);
    bool Connect();
    bool Valid()const;
    static std::pair<int, sockaddr_union> CreateSocket(const string& addr, int port);
    ~Connection();
    string RecvMsg();
    void SendMsg(const string &msg);

    void SendProcessImage(const ProcessImage&);
    ProcessImage RecvProcessImage(fs::path targetFileLocation);
};
