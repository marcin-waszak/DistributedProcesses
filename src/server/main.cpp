#include "../common/connection.h"

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

bool serverListenTest(unsigned port) {
    /*
    * This is temporary implementation of server for debugging
    * TODO: implement properly
    * */
    struct sockaddr_in sa;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        return false;
    }

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(SocketFD,(struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("bind failed");
        close(SocketFD);
        return false;
    }

    if (listen(SocketFD, 10) == -1) {
        perror("listen failed");
        close(SocketFD);
        return false;
    }

    for (;;) {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        printf("new client \n");

        if (0 > ConnectFD) {
            perror("accept failed");
            close(SocketFD);
            continue;
        }

        // Example behavior
        Connection c(ConnectFD);
        string msg = c.recvMsg();
        std::cout<< "got command: " << msg << std::endl;
        string exampleResponse = "123";
        std::cout<< "responding: " << exampleResponse << std::endl;
        c.sendMsg(exampleResponse);
    }

    close(SocketFD);
    return true;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("port,p", po::value<int>()->default_value(1100), "Port to listen at")
    ;
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

    if (serverListenTest(port))  {
        cout << "Closing server" << endl;
        return 0;
    } else {
        cout << "Rerver run failed" << endl;
        return 1;
    }

}
