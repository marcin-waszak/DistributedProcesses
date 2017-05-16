#pragma once
#include "worker.h"
#include "../common/connection.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection : Connection{
public:
    AdminServerConnection(const string&, int port);
    vector<shared_ptr<Worker>> getWorkers();
};
