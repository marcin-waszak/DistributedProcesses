//
// Created by marcin on 23.05.17.
//

#include "Server.h"

Server::Server() : session_id_(0) {

}

void Server::ThreadFunc(int connect_fd, unsigned session_id) {
  Connection connection(connect_fd);

  for (;;) {
    if (!ExecCmd(connection))
      break;

    break; // TODO: delete, when Recv() fixed
  }

  // TODO: synchronization
  sessions_to_close_mutex_.lock();
  sessions_to_close_.push_back(session_id);
  sessions_to_close_mutex_.unlock();
}

bool Server::CleanThreads() { // TODO: sometimes crashes due to nonexistent map key
  sessions_to_close_mutex_.lock();

  while(!sessions_to_close_.empty()) {
    auto session_id = sessions_to_close_.back();
    if(sessions_.at(session_id)->joinable())
      sessions_.at(session_id)->join();
    sessions_.erase(session_id); // TODO: if not exist return false
    sessions_to_close_.pop_back();
  }

  sessions_to_close_mutex_.unlock();
  return true;
}

bool Server::ExecCmd(Connection& connection) {
  string msg = connection.RecvMsg();
  cout << "got command: " << msg << endl;
  if (msg == "GET_WORKERS") {
    // workers not implemented yet
    string resp = "0";
    cout << "responding: " << resp << endl;
    connection.SendMsg(resp);
  } else if (msg == "GET_IMAGES_LIST") {
    if (process_images_.empty()) {
      string resp = "<empty>";
      connection.SendMsg(resp);
      cout << "responding:" << resp << endl;
    } else {
      ostringstream oss;
      for (ProcessImage& pi : process_images_) {
        oss << pi.GetPath() << endl;
      }
      cout << "responding:\n" << oss.str() << endl;
      connection.SendMsg(oss.str());
    }
  } else if (msg == "UPLOAD_IMAGE") {
    string name = connection.RecvMsg();
    fs::path filePath = images_path_ / name;
    ProcessImage pi = connection.RecvProcessImage(filePath);
    cout << "image saved: " << filePath << endl;
    bool found = false;
    for (auto& p : process_images_) {
      if (p.GetPath() == pi.GetPath()) {
        found = true;
        break;
      }
    }
    if (!found)
      process_images_.push_back(pi);
  } else {
    cout << "unknown command, ignoring" << endl;
  }

  // false on connection close
  return true;
}

bool Server::ServerLoop() {
  /*
  * This is temporary implementation of server for debugging
  * TODO: implement properly
  * */
  auto soc = Connection::CreateSocket(address_str_, port_);
  int socket_fd = soc.first;
  sockaddr_union sa = soc.second;

  int optval;
  int optlen;
  // see if the SO_REUSEADDR flag is set:
  getsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t*)&optlen);
  if (optval != 0) {
    cerr << "SO_REUSEADDR enabled!\n";
  }

  if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    perror("bind failed");

    if(close(socket_fd) < 0)
      perror("close failed");

    return false;
  }

  cerr << "Bind success\n";

  if (listen(socket_fd, 10) < 0) {
    perror("listen failed");

    if(close(socket_fd) < 0)
      perror("close failed");

    return false;
  }

  cerr << "Listen success\n";

  for (;;) {
    CleanThreads();

    int connect_fd = accept(socket_fd, NULL, NULL);

    if (0 > connect_fd) {
      perror("accept failed");

      if(close(socket_fd) < 0) {
        perror("close failed");
        break;
      }

      continue;
    }

    cerr << "Accept success\n";
    cout << "----new client---" << endl;

    sessions_to_close_mutex_.lock();
    ++session_id_;
    sessions_to_close_mutex_.unlock();

    sessions_.insert(make_pair(session_id_, make_unique<thread>(
        thread(&Server::ThreadFunc, this, connect_fd, session_id_))));

  }

  if(close(socket_fd) < 0)
    perror("close failed");

  cerr << "Closing server!\n";
  return true;
}

void Server::GetArguments(int argc, char** argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "print help message")
      ("images-path,i", po::value<string>()->default_value("_server_images"),
       "relative path to directory with process images")
      ("address,ap", po::value<string>()->default_value("localhost"), "Server address")
      ("port,p", po::value<int>()->default_value(1100), "Port to listen at");
  po::positional_options_description pd;
  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(pd).run(), vm);
    po::notify(vm);

    images_path_ = vm["images-path"].as<string>();
    if (!fs::exists(images_path_)) {
      cout << "Process images directory does not exist, creating empty directory: "
           << images_path_ << endl;
      fs::create_directory(images_path_);
    }

    if (vm.count("help")) {
      cout << desc << endl;
      exit(0);
    }
    address_str_ = vm["address"].as<string>();
    port_ = vm["port"].as<int>();
  }
  catch(po::error& e) {
    cerr << "ERROR: " << e.what() << endl << endl;
    cerr << desc << endl;
    exit(1);
  }
}