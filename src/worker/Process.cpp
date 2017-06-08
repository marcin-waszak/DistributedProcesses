#include "Process.h"

#include <boost/process/pipe.hpp>
#include <boost/process/io.hpp>

#include <boost/filesystem.hpp>

#include  <iostream>

namespace bf = ::boost::filesystem;

namespace Work {

Process::Process(ProcessImage pi)
    : image_(pi), killed_(true) {
    thread_ = std::make_unique<std::thread>(&Process::Manage, this);
    run_mutex_.lock();
}

bool Process::isRunning() {
    //return child_->running();
    return !killed_; //&& child_.running();
}

bool Process::RunNow() {
    //if (child_->running())
    if (!killed_ )
        return false;

    bf::permissions(image_.GetPath(), bf::owner_all|bf::others_all);

    pipe_stream_ = std::make_unique<bp::ipstream>();
    child_ = bp::child(image_.GetPath(),
            bp::std_out > *pipe_stream_,
            bp::std_err > bp::null);
    killed_ = false;
    run_mutex_.unlock();
    return true;
}

bool Process::StopNow() {
    // child_->terminate();
    // return child_->running();
    child_.terminate();
    killed_ = true;
    return true; //child_.running();
}

void Process::Manage() {
    while (1) {
        run_mutex_.lock();
        std::string line;
        while (!killed_ && *pipe_stream_
                        && std::getline(*pipe_stream_, line)
                        && !line.empty()
                        && !killed_) // synchronization
            std::cerr << line << std::endl;
        if (!killed_) // HACK: child.running() is not working
            killed_ = true;
    }
}

}
