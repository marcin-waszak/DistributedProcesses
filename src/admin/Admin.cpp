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

    if (!vm.count("server-addr")) {
      std::cerr << "Server address not given." << endl;
      cout << desc << endl;
      exit(1);
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

void Admin::Connect() {
  connection_ = AdminServerConnection(server_address_, server_port_);

  if (!connection_.Valid()) {
    std::cerr << "Cannot connect to server." << endl;
    exit(1);
  }
}

bool Admin::CommandParser() {
  string command;

  for(;;) {
    cout << "> ";
    getline(cin, command);

    boost::char_separator<char> sep(" \t");
    boost::tokenizer<boost::char_separator<char>> tokens(command, sep);

    auto beg = tokens.begin();

    if (*beg == "exit") {
      connection_.Close();
      break;
    }
    else if (*beg == "list_workers") {
      cout << "Workers count "
           << connection_.GetWorkers().size();
    }
    else if (*beg == "list_images") {
      cout << "Images on server:\n"
           << connection_.GetProcessImagesList() << endl;
    }
    else if (*beg == "upload_image") {
      connection_.SendMsg("UPLOAD_IMAGE");
      ++beg;
      string imageName = *beg;
      connection_.SendMsg(imageName);
      ProcessImage pi(imageName);
      connection_.SendProcessImage(pi);
    }
    else {
      cout << "Invalid command: " << *beg << endl;
    }
  }

  return true;
}