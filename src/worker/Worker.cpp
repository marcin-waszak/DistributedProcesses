#include "Worker.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

Worker::Worker(const string& addr,int port, fs::path images_path):
    server_connection_(addr,port), images_path_(images_path) {
    if (!fs::exists(images_path_)) {
        std::cout << "Process images directory does not exist, creating empty directory: "
                  << images_path_ << std::endl;
        fs::create_directory(images_path_);
    }
    server_connection_.SendMsg("WORKER");
}

void Worker::ExecCmd(const string& msg) {
    std::cout<< "got message:\n" << msg << std::endl;
    if (msg == "GET_IMAGES_LIST") {
        if (process_images_.empty()) {
            string resp = "<empty>";
            server_connection_.SendMsg("GET_IMAGES_LIST_RESPONSE");
            server_connection_.SendMsg(resp);
            std::cout<< "responding:" << resp << std::endl;
        } else {
            std::ostringstream oss;
            for (ProcessImage pi : process_images_) {
                oss << pi.GetPath() << std::endl;
            }
            std::cout<< "responding:\n" << oss.str() << std::endl;
            server_connection_.SendMsg("GET_IMAGES_LIST_RESPONSE");
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
        server_connection_.SendMsg("UPLOAD_IMAGE_RESPONSE");
        // TODO: error
        server_connection_.SendMsg("OK");
    } else if (msg == "DELETE_IMAGE") {
        string name = server_connection_.RecvMsg();
        fs::path filePath = images_path_ / name;
        remove(filePath);
        for (size_t i = 0; i < process_images_.size(); i++) {
            if (process_images_[i].GetPath() == filePath) {
                process_images_.erase(process_images_.begin() + i);
            }
        }
        std::cout << "Image removed: " << filePath << std::endl;
        server_connection_.SendMsg("DELETE_IMAGE_RESPONSE");
        server_connection_.SendMsg("OK");
    } else if (msg == "RUN_NOW") {
        string name = server_connection_.RecvMsg();
        if (processes_.count(name)) {
            if (processes_[name]->isRunning()) {
                server_connection_.SendMsg("RUN_NOW_RESPONSE");
                server_connection_.SendMsg("Process for this image already running");
            } else {
                processes_[name]->RunNow();
                server_connection_.SendMsg("RUN_NOW_RESPONSE");
                server_connection_.SendMsg("OK");
            }
        } else {
            bool found = false;
            for (auto pi : process_images_) {
                if (pi.GetName() == name) {
                    processes_[name] = std::make_unique<Process>(pi);
                    processes_[name]->RunNow();
                    found = true;
                    server_connection_.SendMsg("RUN_NOW_RESPONSE");
                    server_connection_.SendMsg("OK");
                    break;
                }
            }
            if (!found) {
                server_connection_.SendMsg("RUN_NOW_RESPONSE");
                server_connection_.SendMsg("Such image does not exist on worker");
            }
        }
    } else if (msg == "STOP_NOW") {
        string name = server_connection_.RecvMsg();
        if (processes_.count(name)) {
            if (!processes_[name]->isRunning()) {
                server_connection_.SendMsg("STOP_NOW_RESPONSE");
                server_connection_.SendMsg("Process for this image is already not running");
            } else {
                processes_[name]->StopNow();
                server_connection_.SendMsg("STOP_NOW_RESPONSE");
                server_connection_.SendMsg("OK");
            }
        } else {
            bool found = false;
            for (auto pi : process_images_) {
                if (pi.GetName() == name) {
                    found = true;
                    server_connection_.SendMsg("STOP_NOW_RESPONSE");
                    server_connection_.SendMsg("Process for this image is already not running");
                    break;
                }
            }
            if (!found) {
                server_connection_.SendMsg("RUN_NOW_RESPONSE");
                server_connection_.SendMsg("Such image does not exist on worker");
            }
        }
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
        } catch (ConnectionException) {
            std::cout<< "Trying to establish server connection." << std::endl;
            if (server_connection_.Connect()) {
                std::cout<< "Connection established." << std::endl;
                try {
                    server_connection_.SendMsg("WORKER");
                } catch (ConnectionException) {
                    std::cerr<< "Connection error during handshake." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }
    }
    return true;
}
