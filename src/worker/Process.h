#pragma once

#include "../common/ProcessImage.h"

class Process {
    ProcessImage image_;
    bool should_be_running_;
public:
    Process(ProcessImage);

    bool isRunning();

    bool RunNow();
    bool StopNow();
};
