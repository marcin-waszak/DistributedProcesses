#include "AdminServerConnection.h"

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

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("list-workers,l", "list workers")
        ("list-images", "list process images deployed on server")
        ("upload-image,u", po::value<string>(), "upload image server (local file path should be given)")
        ("server-addr", po::value<string>(), "server address (ipv4 or ipv6)")
        ("server-port,p", po::value<int>()->default_value(1100), "server port");
    po::positional_options_description pd;
    pd.add("server-addr", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
          options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }

    if (vm.count("list-workers")) {
        AdminServerConnection connection = CreateServerConnection(vm);
        cout << "Workers count " << connection.GetWorkers().size() << endl;
        return 0;
    }

    if (vm.count("list-images")) {
        AdminServerConnection connection = CreateServerConnection(vm);
        cout << "Images on server:\n"<< connection.GetProcessImagesList() << endl;
        return 0;
    }

    if (vm.count("upload-image")) {
        AdminServerConnection connection = CreateServerConnection(vm);
        connection.SendMsg("UPLOAD_IMAGE");
        string imageName = vm["upload-image"].as<string>();
        connection.SendMsg(imageName);
        ProcessImage pi(imageName);
        connection.SendProcessImage(pi);
        return 0;
    }

    cout << desc << endl;
    return 0;
}
