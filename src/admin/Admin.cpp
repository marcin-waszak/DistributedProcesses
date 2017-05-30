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
      ("debug,d", "debug mode")
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
      Log::Error("Server address is not given.");
      cout << desc << endl;
      exit(1);
    }

    Log::Enable = !vm_.count("debug");

    server_address_ = vm_["server-addr"].as<string>();
    server_port_ = vm_["server-port"].as<int>();
  }
  catch(po::error& e) {
    Log::Error("ERROR: %s",e.what());
    cout << desc << endl;
    exit(1);
  }
}

void Admin::Connect() {
  connection_ = make_unique<AdminServerConnection>(server_address_, server_port_);

  if (!connection_->Valid()) {
    Log::Error("Cannot connect to server.");
    exit(1);
  }
}

bool Admin::IsInteractive() {
  return static_cast<bool>(vm_.count("interactive"));
}

void Admin::BatchMode() {
  if (vm_.count("list-workers")) {
    Log::Info("Workers list:");
    Log::Out(connection_->GetWorkers().c_str());
  }
  if (vm_.count("list-images")) {
    Log::Info("Images on server:");
    Log::Out(connection_->GetProcessImagesList().c_str());
  }
  if (vm_.count("upload-image")) {
    string image_path = vm_["upload-image"].as<string>();
    UploadImage(image_path);
  }
}

bool Admin::CommandParser() {
  const char* prompt = "> ";
  char *input;

  rl_bind_key('\t', rl_complete);

  for (;;) {
    input = readline(prompt);
    add_history(input);

    Log::Info("%s",input);

    if (!input || !strcmp(input, "exit")) {
      connection_->Close();
      break;
    }
    ParseCommand(input);
  }
  return true;
}

void Admin::ParseCommand(string command) {

  std::vector<std::string> elems;
  boost::split(elems, command, boost::is_any_of("\t "));

  string cmd = elems[0];
  boost::trim(cmd);

  if (cmd == "list_workers") {
    ListWorkers();
  }
  else if (cmd == "list_images") {
    ListImages();
  }
  else if (cmd == "upload_image") {
    string image_path = command.substr(command.find(" ") + 1);
    UploadImage(image_path);
  }
  else if (cmd == "delete_image") {
    DeleteImage(elems);
  }
  else if (cmd == "list_workers_images") {
    ListWorkersImages();
  }
  else if (cmd == "upload_image_worker") {
    UploadImageWorker(elems);
  }
  else if (cmd == "delete_image_worker") {
    DeleteImageWorker(elems);
  }
  else if (cmd == "run_now") {
    RunNow(elems);
  }
  else if (cmd == "stop_now") {
    StopNow(elems);
  }
  else {
    Log::Info("Invalid command: %s", command.c_str());
  }
}

void Admin::ListWorkers() const {
  Log::Info("Workers list:");
  Log::Out(connection_->GetWorkers().c_str());
}

void Admin::ListImages() const {
  Log::Info("Images on server:");
  Log::Out(connection_->GetProcessImagesList().c_str());
}

void Admin::UploadImage(const string &imagePath) const {
  connection_->UploadImage(imagePath);
}

void Admin::ListWorkersImages() const {
  Log::Info("Workers images list:");
  Log::Out(connection_->GetWorkersImages().c_str());
}

void Admin::UploadImageWorker(const vector<string> &elems) const {
  if (elems.size() != 3) {
    Log::Error("Wrong arguments count");
    return;
  }
  Log::Out(connection_->UploadImageWorker(elems[1], elems[2]).c_str());
}

void Admin::DeleteImage(const vector<string> &elems) const {
  if (elems.size() != 2) {
    Log::Error("Wrong arguments count");
    return;
  }
  Log::Out(connection_->DeleteImage(elems[1]).c_str());
}

void Admin::DeleteImageWorker(const vector<string> &elems) const {
  if (elems.size() != 3) {
    Log::Error("Wrong arguments count");
    return;
  }
  Log::Out(connection_->DeleteImageWorker(elems[1], elems[2]).c_str());
}

void Admin::RunNow(const vector<string> &elems) const {
  if (elems.size() != 3) {
    Log::Error("Wrong arguments count");
    return;
  }
  Log::Out(connection_->RunNow(elems[1], elems[2]).c_str());
}
void Admin::StopNow(const vector<string> &elems) const {
  if (elems.size() != 3) {
    Log::Error("Wrong arguments count");
    return;
  }
  Log::Out(connection_->StopNow(elems[1], elems[2]).c_str());
}
