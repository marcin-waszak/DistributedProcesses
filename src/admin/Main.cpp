#include "AdminServerConnection.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
    // TODO: implement properly

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("list-workers,l", "list workers")
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
        if (!vm.count("server-addr")) {
            std::cerr << "Server address not given." << endl;
            return 1;
        }
        string serverAddr = vm["server-addr"].as<string>();
        int server_port = vm["server-port"].as<int>();
        AdminServerConnection connection(serverAddr, server_port);
        std::cout << "Workers count " << connection.GetWorkers().size() << endl;
        return 0;
    }

    cout << desc << endl;
    return 0;
}
