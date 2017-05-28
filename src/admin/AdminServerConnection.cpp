#include "AdminServerConnection.h"

#include <iostream>
#include <boost/algorithm/string.hpp>


AdminServerConnection::AdminServerConnection(const string& address, int port)
    : Connection(address, port) {
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

void AdminServerConnection::UploadImage(string imagePath) {
    boost::trim(imagePath);
    if (imagePath.empty() || !boost::filesystem::exists(imagePath)) {
        std::cout << "File does not exist: " << imagePath << std::endl;
      return;
    }
    SendMsg("UPLOAD_IMAGE");
    string imageName = boost::filesystem::path(imagePath).filename().string();
    SendMsg(imageName);
    ProcessImage pi(imagePath);
    SendProcessImage(pi);
}

bool AdminServerConnection::Close() {
    SendMsg("CLOSE");
    Connection::Close();
    return true;
}
