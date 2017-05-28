//
// Created by marcin on 27.05.17.
//

#include "Admin.h"
#include <boost/algorithm/string.hpp>
#include <readline/readline.h>
#include <readline/history.h>


void Admin::GetArguments(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "print help message")
      ("interactive,i", "interactive mode")
      ("server-addr,a", po::value<string>(), "server address (ipv4 or ipv6)")
      ("server-port,p", po::value<int>()->default_value(1100), "server port")
      ("list-workers,l", "list workers")
      ("list-images,g", "list process images deployed on server")
      ("upload-image,u", po::value<string>(), "upload image server (local file path should be given)");
  po::positional_options_description pd;
  pd.add("server-addr", 1);

  try {
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(pd).run(), vm_);
    po::notify(vm_);

    if (vm_.count("help")) {
      cout << desc << endl;
      exit(0);
    }

    if (!vm_.count("server-addr")) {
      std::cerr << "Server address not given." << endl;
      cout << desc << endl;
      exit(1);
    }

    server_address_ = vm_["server-addr"].as<string>();
    server_port_ = vm_["server-port"].as<int>();
  }
  catch(po::error& e) {
    cerr << "ERROR: " << e.what() << endl << endl;
    cerr << desc << endl;
    exit(1);
  }
}

void Admin::Connect() {
  connection_ = make_unique<AdminServerConnection>(server_address_, server_port_);

  if (!connection_->Valid()) {
    std::cerr << "Cannot connect to server." << endl;
    exit(1);
  }
}

bool Admin::IsInteractive() {
  return static_cast<bool>(vm_.count("interactive"));
}

void Admin::BatchMode() {
  // TODO: get rid of copy-paste code
  if (vm_.count("list-workers"))
    cout << "Workers list:\n"
         << connection_->GetWorkers();

  if (vm_.count("list-images"))
    cout << "Images on server:\n"
         << connection_->GetProcessImagesList() << endl;
  if (vm_.count("upload-image")) {
    string image_path = vm_["upload-image"].as<string>();
    connection_->UploadImage(image_path);
  }
}

bool Admin::CommandParser() {
  const char* prompt = "> ";
  char *input;

  rl_bind_key('\t', rl_complete);

  for (;;) {
    cout << "\n>";
    input = readline(prompt);
    add_history(input);

    cout << input << endl;

    if (!input || !strcmp(input, "exit")) {
      connection_->Close();
      break;
    }

    parseCommand(input);
  }

  return true;
}

void Admin::parseCommand(string command) {
  std::vector<std::string> elems;
  boost::split(elems, command, boost::is_any_of("\t "));

  string cmd = elems[0];
  boost::trim(cmd);
  if (cmd == "list_workers") {
    cout << "Workers list:\n"
         << connection_->GetWorkers();
  }
  else if (cmd == "list_images") {
    cout << "Images on server:\n"
         << connection_->GetProcessImagesList() << endl;
  }
  else if (cmd == "upload_image") {
    string image_path = command.substr(command.find(" ") + 1);
    cout << connection_->UploadImage(image_path) << endl;
  }
  else if (cmd == "list_workers_images") {
    cout << "Workers images list:\n"
         << connection_->GetWorkersImages();
  }
  else if (cmd == "upload_image_worker") {
    if (elems.size() != 3)
      cerr << "Wrong arguments count" << endl;
    else
      cout << connection_->UploadImageWorker(elems[1], elems[2]) << endl;
  }
  else {
    cout << "Invalid command: " << command << endl;
  }
}
