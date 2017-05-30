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

string AdminServerConnection::UploadImage(string imagePath) {
    boost::trim(imagePath);
    if (imagePath.empty() || !boost::filesystem::exists(imagePath)) {
        std::cout << "File does not exist: " << imagePath << std::endl;
        return "File does not exist";
    }
    SendMsg("UPLOAD_IMAGE");
    string imageName = boost::filesystem::path(imagePath).filename().string();
    SendMsg(imageName);
    ProcessImage pi(imagePath);
    SendProcessImage(pi);
    return RecvMsg();
}

string AdminServerConnection::DeleteImage(string imageName) {
    boost::trim(imageName);
    SendMsg("DELETE_IMAGE");
    SendMsg(imageName);
    return RecvMsg();
}

string AdminServerConnection::GetWorkersImages() {
    SendMsg("GET_WORKERS_IMAGES");
    return RecvMsg();
}

bool AdminServerConnection::Close() {
    SendMsg("CLOSE");
    Connection::Close();
    return true;
}

string AdminServerConnection::UploadImageWorker(string imageName, string workerId) {
    boost::trim(imageName);
    boost::trim(workerId);
    SendMsg("UPLOAD_IMAGE_WORKER");
    SendMsg(imageName);
    SendMsg(workerId);
    return RecvMsg();
}

string AdminServerConnection::DeleteImageWorker(string imageName, string workerId) {
    boost::trim(imageName);
    boost::trim(workerId);
    SendMsg("DELETE_IMAGE_WORKER");
    SendMsg(imageName);
    SendMsg(workerId);
    return RecvMsg();
}

string AdminServerConnection::RunNow(string imageName, string workerId) {
    boost::trim(imageName);
    boost::trim(workerId);
    SendMsg("RUN_NOW");
    SendMsg(imageName);
    SendMsg(workerId);
    return RecvMsg();
}

string AdminServerConnection::StopNow(string imageName, string workerId) {
    boost::trim(imageName);
    boost::trim(workerId);
    SendMsg("STOP_NOW");
    SendMsg(imageName);
    SendMsg(workerId);
    return RecvMsg();
}
