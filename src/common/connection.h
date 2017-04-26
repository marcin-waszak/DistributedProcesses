#pragma once

#include <string>
#include <netinet/in.h>

using std::string;

class Connection {
    struct sockaddr_in sa;
    int res;
    int SocketFD;

public:
    Connection(const string&);
    ~Connection();
    string receive();
    void sendMsg(const string&);
};
