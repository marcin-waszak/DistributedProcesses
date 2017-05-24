#include "Worker.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

Worker::Worker(const string& addr,int port, fs::path images_path):
    server_connection_(addr,port), images_path_(images_path) {
    // TODO: say server that you are worker
    if (!fs::exists(images_path_)) {
        std::cout << "Process images directory does not exist, creating empty directory: "
                  << images_path_ << std::endl;
        fs::create_directory(images_path_);
    }
}

void Worker::ExecCmd(const string& msg) {
    std::cout<< "got message:\n" << msg << std::endl;
    if (msg == "GET_IMAGES_LIST") {
        if (process_images_.empty()) {
            string resp = "<empty>";
            server_connection_.SendMsg(resp);
            std::cout<< "responding:" << resp << std::endl;
        } else {
            std::ostringstream oss;
            for (ProcessImage pi : process_images_) {
                oss << pi.GetPath() << std::endl;
            }
            std::cout<< "responding:\n" << oss.str() << std::endl;
            server_connection_.SendMsg(oss.str());
        }
    } else if (msg == "UPLOAD_IMAGE") {
        string name = server_connection_.RecvMsg();
        fs::path filePath = images_path_ / name;
        ProcessImage pi = server_connection_.RecvProcessImage(filePath);
        std::cout<< "image saved: " << filePath << std::endl;
        bool found = false;
        for (auto p : process_images_) {
            if (p.GetPath() == pi.GetPath()) {
                found = true;
                break;
            }
        }
        if (!found)
            process_images_.push_back(pi);
    } else {
        std::cout<< "unknown command, ignoring" << std::endl;
    }
}

bool Worker::WorkerLoop() {
    while (true) {
        std::cout<< "--------------------" << std::endl;
        try {
            string cmd = server_connection_.RecvMsg();
            ExecCmd(cmd);
        } catch (std::logic_error) {
            std::cout<< "Trying to establish server connection." << std::endl;
            if (server_connection_.Connect()) {
                std::cout<< "Connection established." << std::endl;
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
    }
    return true;
}
