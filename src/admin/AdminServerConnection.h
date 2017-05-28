#pragma once
#include "Worker.h"
#include "../common/Connection.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection : public Connection {
public:
    AdminServerConnection(const string& address, int port);
    string GetWorkers();
    string GetProcessImagesList();
    bool Close() override;
};
