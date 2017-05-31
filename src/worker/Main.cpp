#include "Worker.h"

#include <boost/program_options.hpp>

#include <iostream>

bool Log::Enable = true;

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("images-path,i", po::value<string>()->default_value("_worker_images"),
         "relative path to directory with process images")
        ("server-addr,a", po::value<string>()->default_value("localhost"), "server address (ipv4 or ipv6)")
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

    if (!vm.count("server-addr")) {
        std::cerr << "Server address not given." << endl;
        return 1;
    }

    Work::Worker worker(vm["server-addr"].as<string>(),
                  vm["server-port"].as<int>(),
                  vm["images-path"].as<string>());
    if (worker.WorkerLoop()) {
        cout << "Closing worker" << endl;
        // TODO: serialize state
        return 0;
    } else {
        cout << "Worker run failed" << endl;
        return 1;
    }

    cout << desc << endl;
    return 0;
}
