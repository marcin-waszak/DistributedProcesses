#include "../common/Connection.h"

#include <boost/program_options.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

// TODO: refactor: make class (or classes), not global variables,
fs::path imagesPath;
//  also serialize this vector
std::vector<ProcessImage> processImages;

void execCmd(Connection& connection) {
    string msg = connection.RecvMsg();
    std::cout<< "got command: " << msg << std::endl;
    if (msg == "GET_WORKERS") {
        // workers not implemented yet
        string resp = "0";
        std::cout<< "responding: " << resp << std::endl;
        connection.SendMsg(resp);
    } else if (msg == "GET_IMAGES_LIST") {
        if (processImages.empty()) {
            string resp = "<empty>";
            connection.SendMsg(resp);
            std::cout<< "responding:" << resp << std::endl;
        } else {
            std::ostringstream oss;
            for (ProcessImage pi : processImages) {
                oss << pi.GetPath() << endl;
            }
            std::cout<< "responding:\n" << oss.str() << std::endl;
            connection.SendMsg(oss.str());
        }
    } else if (msg == "UPLOAD_IMAGE") {
        string name = connection.RecvMsg();
        fs::path filePath = imagesPath / name;
        ProcessImage pi = connection.RecvProcessImage(filePath);
        std::cout<< "image saved: " << filePath << std::endl;
        bool found = false;
        for (auto p : processImages) {
            if (p.GetPath() == pi.GetPath()) {
                found = true;
                break;
            }
        }
        if (!found)
            processImages.push_back(pi);
    }
}
bool ServerLoop(unsigned port) {
    /*
    * This is temporary implementation of server for debugging
    * TODO: implement properly
    * */
    struct sockaddr_in sa;
    int socket_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd_ == -1) {
        perror("cannot create socket");
        return false;
    }

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd_,(struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("bind failed");

        if(close(socket_fd_) == -1)
            perror("close failed");

        return false;
    }

    if (listen(socket_fd_, 10) == -1) {
        perror("listen failed");

        if(close(socket_fd_) == -1)
            perror("close failed");

        return false;
    }

    for (;;) {
        int connect_fd_ = accept(socket_fd_, NULL, NULL);

        cout << "----new client---" << endl;

        if (0 > connect_fd_) {
            perror("accept failed");

            if(close(socket_fd_) == -1)
                perror("close failed");

            continue;
        }

        Connection connection(connect_fd_);
        execCmd(connection);
    }

    if(close(socket_fd_) == -1)
        perror("close failed");

    return true;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("images-path,i", po::value<string>()->default_value("_server_images"),
            "relative path to directory with process images")
        ("port,p", po::value<int>()->default_value(1100), "Port to listen at");
    po::positional_options_description pd;
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
          options(desc).positional(pd).run(), vm);
    po::notify(vm);

    imagesPath = vm["images-path"].as<string>();
    if (!fs::exists(imagesPath)) {
        cout << "Process images directory does not exist, creating empty directory: "
             << imagesPath << endl;
        fs::create_directory(imagesPath);
    }

    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }
    int port = vm["port"].as<int>();

    if (ServerLoop(port))  {
        cout << "Closing server" << endl;
        // TODO: serialize state
        return 0;
    } else {
        cout << "Server run failed" << endl;
        return 1;
    }

}
