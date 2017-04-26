#pragma once
#include "worker.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection {
public:
    AdminServerConnection() {}
    vector<shared_ptr<Worker>> getWorkers();
};
