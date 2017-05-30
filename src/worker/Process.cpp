#include "Process.h"

Process::Process(ProcessImage pi)
    : image_(pi), should_be_running_(false) {
}

bool Process::isRunning() {
    // TODO: implement real process runnning
    return should_be_running_;
}

bool Process::RunNow() {
    // TODO: implement real process runnning
    should_be_running_ = 1;
    return true;
}

bool Process::StopNow() {
    should_be_running_ = 0;
    return true;
}
