#pragma once

#include "../common/Connection.h"

#include <thread>
#include <mutex>
using std::thread;
using std::mutex;
using std::unique_ptr;
using std::make_unique;

class Server;

class Worker {
    Connection connection_;
    Server& server_;
    unique_ptr<thread> thread_;
    bool closed_;
    mutex access_;
    string result_; // TODO: maybe more complex data
    mutex response_;

    void Loop();
public:
    explicit Worker(int connect_id, Server&);
    ~Worker();

    // query methods
    string ListImages();

    bool Closed();
};
