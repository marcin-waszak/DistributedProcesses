#include "AdminServerConnection.h"
#include "Admin.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

AdminServerConnection CreateServerConnection(po::variables_map vm) {
    if (!vm.count("server-addr")) {
        std::cerr << "Server address not given." << endl;
        exit(1);
    }
    string serverAddr = vm["server-addr"].as<string>();
    int server_port = vm["server-port"].as<int>();
    return AdminServerConnection(serverAddr, server_port);
}

int main(int argc, char* argv[]) {
    // TODO: implement properly

    Admin admin;

    admin.GetArguments(argc, argv);

    admin.CommandParser();


//    if (vm.count("list-workers")) {
//        AdminServerConnection connection = CreateServerConnection(vm);
//        if (!connection.Valid()) {
//            std::cerr << "Cannot connect to server." << endl;
//            return 1;
//        }
//        cout << "Workers count " << connection.GetWorkers().size() << endl;
//        return 0;
//    }
//
//    if (vm.count("list-images")) {
//        AdminServerConnection connection = CreateServerConnection(vm);
//        if (!connection.Valid()) {
//            std::cerr << "Cannot connect to server." << endl;
//            return 1;
//        }
//        cout << "Images on server:\n"<< connection.GetProcessImagesList() << endl;
//        return 0;
//    }
//
//    if (vm.count("upload-image")) {
//        AdminServerConnection connection = CreateServerConnection(vm);
//        if (!connection.Valid()) {
//            std::cerr << "Cannot connect to server." << endl;
//            return 1;
//        }
//        connection.SendMsg("UPLOAD_IMAGE");
//        string imageName = vm["upload-image"].as<string>();
//        connection.SendMsg(imageName);
//        ProcessImage pi(imageName);
//        connection.SendProcessImage(pi);
//        return 0;
//    }

//    cout << desc << endl;
    return 0;
}
