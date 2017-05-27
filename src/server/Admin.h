#pragma once

#include "../common/Connection.h"

#include <thread>
using std::thread;
using std::unique_ptr;
using std::make_unique;

class Server;

class Admin {
    Connection connection_;
    Server& server_;
    unique_ptr<thread> thread_;
    bool closed_;

    bool ExecCmd();
    void Loop();
public:
    explicit Admin(int connect_id, Server&);
    ~Admin();

    bool Closed();
};
