#pragma once
#include "Worker.h"
#include "../common/Connection.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection : public Connection{
public:
    AdminServerConnection();
    AdminServerConnection(const string&, int port);
    vector<shared_ptr<Worker>> GetWorkers();
    string GetProcessImagesList();
};
