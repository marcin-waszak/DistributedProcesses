//
// Created by marcin on 23.05.17.
//

#pragma once

#include "Admin.h"
#include "Worker.h"
#include "../common/Connection.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <thread>
#include <memory>
#include <mutex>

namespace po = boost::program_options;

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::make_pair;
using std::make_unique;
using std::unique_ptr;
using std::thread;
using std::ostringstream;
using std::map;
using std::vector;
using std::mutex;

class Server {
public:
  Server();
  void GetArguments(int argc, char** argv);
  bool ServerLoop();
  bool CleanThreads();
  vector<ProcessImage> GetProcessImages();
  fs::path GetImagesPath() { return images_path_; }
  void AddProcessImage(ProcessImage);

private:

  string address_str_;
  int port_;
  fs::path images_path_;

  mutex process_images_mutex_;
  vector<ProcessImage> process_images_;

  map<int, unique_ptr<Admin>> admins_;
  map<int, unique_ptr<Worker>> workers_;

  int next_admin_id_;
  int next_worker_id_;
};
