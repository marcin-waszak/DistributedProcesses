#include "../common/Connection.h"

#include <boost/program_options.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

union sockaddr_union {
  sockaddr_in6 sin6;
  sockaddr_in sin;
};

// TODO: refactor: make class (or classes), not global variables,
string address;
int port;
fs::path images_path;
//  also serialize this vector
std::vector<ProcessImage> processImages;

int Resolve(const string &address, addrinfo** info) {
  int result = getaddrinfo(address.c_str(), NULL, NULL, info);

  if (result) {
    cerr << "Invalid address" << endl
         << gai_strerror(result) << endl;
    return -1;
  }

  return 0;
}

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
        fs::path filePath = images_path / name;
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

bool ServerLoop(const string& address, int port) {
    /*
    * This is temporary implementation of server for debugging
    * TODO: implement properly
    * */
    addrinfo* info = NULL;
    if(Resolve(address, &info) < 0) {
      cerr << "Cannot resolve()" << endl;
      return false;
    }

    cerr << "Resolve success\n";

    int address_format = info->ai_family;
    int packet_format = address_format == AF_INET6 ? PF_INET6 : PF_INET;
    int socket_fd = socket(packet_format, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      perror("socket()");
      return false;
    }

    sockaddr_union sa;
    memset(&sa, 0, sizeof(sa));
    sockaddr_in* addr_info;   // TODO: try to use sockaddr_union
    sockaddr_in6* addr_info6; //

    switch(address_format) {
      case AF_INET:
        addr_info = (sockaddr_in*)info->ai_addr;
        sa.sin.sin_family = AF_INET;
        sa.sin.sin_port = htons(port);
        sa.sin.sin_addr = addr_info->sin_addr;
        break;

      case AF_INET6:
        addr_info6 = (sockaddr_in6*)info->ai_addr;
        sa.sin6.sin6_family = AF_INET6;
        sa.sin6.sin6_port = htons(port);
        sa.sin6.sin6_addr = addr_info6->sin6_addr;
        break;

      default:
        return false;
    }

//  freeaddrinfo(info);

    if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind failed");

        if(close(socket_fd) < 0)
            perror("close failed");

        return false;
    }

    cerr << "Bind success\n";

    if (listen(socket_fd, 10) < 0) {
        perror("listen failed");

        if(close(socket_fd) < 0)
            perror("close failed");

        return false;
    }

    cerr << "Listen success\n";

    for (;;) {
        int connect_fd = accept(socket_fd, NULL, NULL);

        if (0 > connect_fd) {
            perror("accept failed");

            if(close(socket_fd) < 0)
                perror("close failed");

            continue;
        }
        cerr << "Accept success\n";
        cout << "----new client---" << endl;
        Connection connection(connect_fd);
        execCmd(connection);
    }

    if(close(socket_fd) < 0)
        perror("close failed");

    return true;
}

void GetArguments(int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("images-path,i", po::value<string>()->default_value("_server_images"),
         "relative path to directory with process images")
        ("address,ap", po::value<string>()->default_value("localhost"), "Server address")
        ("port,p", po::value<int>()->default_value(1100), "Port to listen at");
    po::positional_options_description pd;
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).
            options(desc).positional(pd).run(), vm);
        po::notify(vm);

        images_path = vm["images-path"].as<string>();
        if (!fs::exists(images_path)) {
            cout << "Process images directory does not exist, creating empty directory: "
                 << images_path << endl;
            fs::create_directory(images_path);
        }

        if (vm.count("help")) {
            cout << desc << endl;
            exit(0);
        }
        address = vm["address"].as<string>();
        port = vm["port"].as<int>();
    }
    catch(po::error& e) {
        cerr << "ERROR: " << e.what() << endl << endl;
        cerr << desc << endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    GetArguments(argc, argv);

    if (ServerLoop(address, port))  {
        cout << "Closing server" << endl;
        // TODO: serialize state
        return 0;
    } else {
        cout << "Server run failed" << endl;
        return 1;
    }
}
