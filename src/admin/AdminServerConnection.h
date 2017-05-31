#pragma once
#include "../common/Connection.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AdminServerConnection : public Connection {
public:
    AdminServerConnection(const string& address, int port);

    // commands
    string GetWorkers();
    string GetProcessImagesList();
    string UploadImage(string path);
    string DeleteImage(string imageName);
    string GetWorkersImages();
    string UploadImageWorker(string imageName, string workerId);
    string DeleteImageWorker(string imageName, string workerId);
    string RunNow(string imageName, string workerId);
    string StopNow(string imageName, string workerId);

    bool Close() override;
};
