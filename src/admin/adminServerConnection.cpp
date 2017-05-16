#include "adminServerConnection.h"


AdminServerConnection::AdminServerConnection(const string& serverAddr, int port) : Connection(serverAddr,port) {
    // TODO: implement properly
}

vector<shared_ptr<Worker>> AdminServerConnection::getWorkers() {
    // TODO: implement properly
    sendMsg("GET_WORKERS");
    string r = recvMsg();
    return vector<shared_ptr<Worker>>(std::stoi(r), nullptr);
}
