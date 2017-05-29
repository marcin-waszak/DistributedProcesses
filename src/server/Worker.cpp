#include "Worker.h"

#include "Server.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

Worker::Worker(unique_ptr<Connection> connection, Server& server)
        : connection_(std::move(connection)),
        server_(server),
        closed_(false) {
    thread_ = make_unique<thread>(&Worker::Loop, this);
    response_.lock();
}

Worker::~Worker() {
    assert(closed_);
    thread_->join();

    // there may be still waiting consument
    response_.unlock();
}

void Worker::Loop() {
    while (true) {
        try {
            string msg = connection_->RecvMsg();
            std::cout << "Got response from worker: " << msg << endl;
            if (msg == "GET_IMAGES_LIST_RESPONSE") {
                result_ = connection_->RecvMsg();
                response_.unlock();
            } else if (msg == "UPLOAD_IMAGE_RESPONSE") {
                result_ = connection_->RecvMsg();
                response_.unlock();
            } else if (msg == "WORKER_ERROR") {
                std::cout << "Worker raised error" << endl;
                // TODO
            } else {
                std::cerr << "Critical error: Invalid worker response." << endl;
            }
        } catch (ConnectionException) {
            cerr << "Worker disconnected" << endl;
            closed_ = true;
            break;
        }
    }
}

string Worker::ListImages() {
    std::lock_guard<std::mutex> lock(access_);
    string res = "error";
    try {
      connection_->SendMsg("GET_IMAGES_LIST");
    } catch (ConnectionException) {
      closed_ = true;
    }
    if (closed_)
        return res;
    response_.lock();
    res = result_;
    return res;
}

string Worker::UploadImage(ProcessImage p) {
    std::lock_guard<std::mutex> lock(access_);
    string res = "error";
    try {
      connection_->SendMsg("UPLOAD_IMAGE");
      connection_->SendMsg(p.GetPath().filename().string());
      connection_->SendProcessImage(p);
    } catch (ConnectionException) {
      closed_ = true;
    }
    if (closed_)
        return res;
    response_.lock();
    res = result_;
    return res;
}

string Worker::DeleteImage(ProcessImage p) {
    std::lock_guard<std::mutex> lock(access_);
    string res = "error";
    try {
        connection_->SendMsg("DELETE_IMAGE");
        connection_->SendMsg(p.GetPath().filename().string());
        connection_->SendProcessImage(p);
    } catch (ConnectionException) {
        closed_ = true;
    }
    if (closed_)
        return res;
    response_.lock();
    res = result_;
    return res;
}

bool Worker::Closed() {
    return closed_;
}

string Worker::GetAddress()const {
    return connection_->GetAddress();
}
