#pragma once

#include <string>
#include <netinet/in.h>

using std::string;

class Connection {
    struct sockaddr_in sa_;
    int socked_fd_;

public:
    Connection(int fd);
    Connection(const string& addr,int port);
    ~Connection();
    string RecvMsg();
    void SendMsg(const string &msg);
};
