#pragma once

#include <string>
#include <netinet/in.h>

using std::string;

class Connection {
    struct sockaddr_in sa;
    int res;
    int SocketFD;

public:
    Connection(int fd);
    Connection(const string& addr,int port);
    ~Connection();
    string recvMsg();
    void sendMsg(const string&);
};
