#pragma once

#include "../common/Connection.h"

#include <thread>
using std::thread;
using std::unique_ptr;
using std::make_unique;

namespace Srv {

class Server;

class Admin {
    unique_ptr<Connection> connection_;
    Server& server_;
    unique_ptr<thread> thread_;
    bool closed_;

    bool ExecCmd();
    void Loop();
public:
    explicit Admin(unique_ptr<Connection>, Server&);
    ~Admin();

    bool Closed();
};

}
