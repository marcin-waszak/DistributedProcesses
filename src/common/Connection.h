#pragma once

#include "ProcessImage.h"

#include <string>
#include <sstream>
#include <netinet/in.h>

using std::string;

union sockaddr_union {
  sockaddr_in6 sin6;
  sockaddr_in sin;
};

class ConnectionException : public std::runtime_error {
public:
    ConnectionException(std::string s)
        :runtime_error(BuildErrorMessage(s)) {
    }

    string BuildErrorMessage(std::string s) {
        return "ConnectionException: " + s;
    }
};

class Connection {
    int socked_fd_;

    // for reconnecting
    string addr_;
    int port_;

    bool valid_;

    Connection(const Connection&) = delete;
    Connection& operator=(Connection const&) = delete;
    void Send(void* data, size_t size);
    void Recv(void* data, size_t size);

public:
    Connection(int fd);
    Connection(const string& addr,int port);
    bool Connect();
    bool Close();
    bool Valid()const;
    static std::pair<int, sockaddr_union> CreateSocket(const string& addr, int port);
    ~Connection();
    string RecvMsg();
    void SendMsg(const string &msg);

    void SendProcessImage(const ProcessImage&);
    ProcessImage RecvProcessImage(fs::path targetFileLocation);
};
