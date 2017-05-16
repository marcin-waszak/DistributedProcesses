#include "AdminServerConnection.h"


AdminServerConnection::AdminServerConnection(const string& serverAddr, int port) : Connection(serverAddr,port) {
    // TODO: implement properly
}

vector<shared_ptr<Worker>> AdminServerConnection::GetWorkers() {
    // TODO: implement properly
    SendMsg("GET_WORKERS");
    string r = RecvMsg();
    return vector<shared_ptr<Worker>>(std::stoi(r), nullptr);
}

string AdminServerConnection::GetProcessImagesList() {
    // TODO: error handling
    SendMsg("GET_IMAGES_LIST");
    return RecvMsg();
}
