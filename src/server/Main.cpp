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

bool ServerListenTest(unsigned port) {
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

        printf("new client \n");

        if (0 > connect_fd_) {
            perror("accept failed");

            if(close(socket_fd_) == -1)
                perror("close failed");

            continue;
        }

        // Example behavior
        Connection connection(connect_fd_);
        string msg = connection.RecvMsg();
        std::cout<< "got command: " << msg << std::endl;
        string exampleResponse = "123";
        std::cout<< "responding: " << exampleResponse << std::endl;
        connection.SendMsg(exampleResponse);
    }

    if(close(socket_fd_) == -1)
        perror("close failed");

    return true;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("port,p", po::value<int>()->default_value(1100), "Port to listen at");
    po::positional_options_description pd;
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
          options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }
    int port = vm["port"].as<int>();

    if (ServerListenTest(port))  {
        cout << "Closing server" << endl;
        return 0;
    } else {
        cout << "Server run failed" << endl;
        return 1;
    }

}
