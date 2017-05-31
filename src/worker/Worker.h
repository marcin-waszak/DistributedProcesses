#pragma once

#include "../common/Connection.h"
#include "Process.h"

#include <map>

namespace Work {

class Worker {
    Connection server_connection_;
    std::vector<ProcessImage> process_images_;
    std::map<string, std::unique_ptr<Process>> processes_;
    fs::path images_path_;

public:
    Worker(const string& addr,int port, fs::path images_path);
    void ExecCmd(const string&);
    bool WorkerLoop();
};

}
