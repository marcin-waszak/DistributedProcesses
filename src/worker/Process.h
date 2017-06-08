#pragma once

#include "../common/ProcessImage.h"

#include <boost/process.hpp>

#include <thread>
namespace bp = ::boost::process;

using std::unique_ptr;
using std::thread;
using std::mutex;

namespace Work {

class Process {
    ProcessImage image_;
    unique_ptr<thread> thread_;

    bp::child child_;
    //unique_ptr<bp::child> child_;
    unique_ptr<bp::ipstream> pipe_stream_;
    //bp::ipstream pipe_stream_;
    bool killed_;

    mutex run_mutex_;
public:
    Process(ProcessImage);

    bool isRunning();

    bool RunNow();
    bool StopNow();

    void Manage();
};

}
