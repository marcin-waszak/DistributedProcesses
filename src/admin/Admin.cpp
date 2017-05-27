//
// Created by marcin on 27.05.17.
//

#include "Admin.h"

void Admin::GetArguments(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "print help message")
      ("server-addr,a", po::value<string>(), "server address (ipv4 or ipv6)")
      ("server-port,p", po::value<int>()->default_value(1100), "server port");
  po::positional_options_description pd;
  pd.add("server-addr", 1);

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << endl;
      exit(0);
    }

    server_address_ = vm["server-addr"].as<string>();
    server_port_ = vm["server-port"].as<int>();
  }
  catch(po::error& e) {
    cerr << "ERROR: " << e.what() << endl << endl;
    cerr << desc << endl;
    exit(1);
  }
}

//TODD: use tokenizer
bool Admin::CommandParser() {
  string command;

  for(;;) {
    cout << "> ";
    getline(cin, command);
    cout << command << endl;

    if(command == "exit")
      return true;

//    break;
  }
}