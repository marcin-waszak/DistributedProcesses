#include "Worker.h"

#include "Server.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

Worker::Worker(int connect_id, Server& srv)
        : connection_(connect_id),
        server_(srv),
        closed_(false) {
    thread_ = make_unique<thread>(thread(&Worker::Loop, this));
}

Worker::~Worker() {
    assert(closed_);
    thread_->join();

    // there may be still waiting consument
    response_.unlock();
}

void Worker::Loop() {
    while (true) {
        string msg = connection_.RecvMsg();
        if (msg == "LIST_IMAGES_RESPONSE") {
            result_ = connection_.RecvMsg();
            response_.unlock();
        } else if (msg == "WORKER_ERROR") {
            std::cout << "Worker raised error" << endl;
            // TODO
        }
    }
}

string Worker::ListImages() {
    std::lock_guard<std::mutex> lock(access_);
    string res = "error";
    if (closed_)
        return res;
    response_.lock();
    res = result_;
    return res;
}

bool Worker::Closed() {
    return closed_;
}
