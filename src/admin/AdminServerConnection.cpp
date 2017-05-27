#include "AdminServerConnection.h"

AdminServerConnection::AdminServerConnection(const string& serverAddr, int port)
    : Connection(serverAddr,port) {
    SendMsg("ADMIN");
}

string AdminServerConnection::GetWorkers() {
    SendMsg("GET_WORKERS");
    return RecvMsg();
}

string AdminServerConnection::GetProcessImagesList() {
    SendMsg("GET_IMAGES_LIST");
    return RecvMsg();
}

bool AdminServerConnection::Close() {
    SendMsg("CLOSE");
    Connection::Close();
    return true;
}
