#pragma once
#include "Worker.h"
#include "../common/Connection.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection : Connection{
public:
    AdminServerConnection(const string&, int port);
    vector<shared_ptr<Worker>> GetWorkers();
};
