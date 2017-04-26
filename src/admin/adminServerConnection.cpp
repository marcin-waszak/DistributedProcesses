#include "adminServerConnection.h"


AdminServerConnection::AdminServerConnection(const string& serverAddr) : Connection(serverAddr) {
    // TODO: implement properly
}

vector<shared_ptr<Worker>> AdminServerConnection::getWorkers() {
    // TODO: implement properly
    sendMsg("GET_WORKERS");
    string r = receive();
    return vector<shared_ptr<Worker>>(std::stoi(r), nullptr);
}
