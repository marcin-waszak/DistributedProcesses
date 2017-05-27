//
// Created by marcin on 23.05.17.
//

#include "Server.h"

Server::Server()
    : next_admin_id_(0), next_worker_id_(0) {

}

bool Server::CleanThreads() {
  for (auto i = admins_.begin(); i != admins_.end(); ) {
      if (i->second->Closed()) {
          std::cout << "Erasing Admin id: " << i->first << endl;
          i = admins_.erase(i);
      } else ++i;
  }
  for (auto i = workers_.begin(); i != workers_.end(); ) {
      if (i->second->Closed()) {
          std::cout << "Erasing Admin id: " << i->first << endl;
          i = workers_.erase(i);
      } else ++i;
  }
  return true;
}

bool Server::ServerLoop() {
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

    // TODO: accept workers
    cout << "----new admin--- id: " << next_admin_id_ << endl;
    Server& srv = *this;
    admins_[next_admin_id_] = make_unique<Admin>(connect_fd, srv);
    ++next_admin_id_;
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
      ("address,a", po::value<string>()->default_value("localhost"), "Server address")
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

vector<ProcessImage> Server::GetProcessImages() {
    std::lock_guard<std::mutex> lock(process_images_mutex_);
    return vector<ProcessImage>(process_images_);
}

void Server::AddProcessImage(ProcessImage pi) {
    std::lock_guard<std::mutex> lock(process_images_mutex_);
    process_images_.push_back(pi);
}
