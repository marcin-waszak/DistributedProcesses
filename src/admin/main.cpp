#include "adminServerConnection.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help message")
        ("list-workers,l", "list workers")
    ;
    po::positional_options_description pd;
    pd.add("input-file", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
          options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }

    if (vm.count("list-workers")) {
        AdminServerConnection conn;
        std::cout << "Workers count " << conn.getWorkers().size() << endl;
        return 0;
    }

    cout << desc << endl;
    return 0;
}
