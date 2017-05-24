//
// Created by marcin on 23.05.17.
//

#pragma once

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
#include <pthread.h>

namespace po = boost::program_options;

using std::cout;
using std::cerr;
using std::endl;
using std::string;

class Server {
public:
  void GetArguments(int argc, char** argv);
  bool ServerLoop();

private:
  void ThreadFunc(int num);
  void ExecCmd(Connection& connection);

  string address_str_;
  int port_;
  fs::path images_path_;
//  also serialize this vector
  std::vector<ProcessImage> process_images_;
};

