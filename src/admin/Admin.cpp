//
// Created by marcin on 27.05.17.
//

#include "Admin.h"
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
    connection_->SendMsg("UPLOAD_IMAGE");
    string imageName = boost::filesystem::path(
        vm_["upload-image"].as<string>()).filename().string();
    connection_->SendMsg(imageName);
    ProcessImage pi(imageName);
    connection_->SendProcessImage(pi);
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
  if (command == "list_workers") {
    cout << "Workers list:\n"
         << connection_->GetWorkers();
  }
  else if (command == "list_images") {
    cout << "Images on server:\n"
         << connection_->GetProcessImagesList() << endl;
  }
  else if (command.find("upload_image") == 0) {
    connection_->SendMsg("UPLOAD_IMAGE");
    string fileName = command.substr(command.find(" ") + 1);
    if (fileName.empty() || !boost::filesystem::exists(fileName)) {
      cout << "File does not exist: " << fileName << endl;
      return;
    }
    string imageName = boost::filesystem::path(fileName).filename().string();
    connection_->SendMsg(imageName);
    ProcessImage pi(imageName);
    connection_->SendProcessImage(pi);
  }
  else {
    cout << "Invalid command: " << command << endl;
  }
}
