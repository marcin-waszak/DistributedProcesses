#pragma once

#include "../common/Connection.h"

class Worker {
    Connection server_connection_;
    std::vector<ProcessImage> process_images_;
    fs::path images_path_;

public:
    Worker(const string& addr,int port, fs::path images_path);
    bool WorkerLoop();
};
