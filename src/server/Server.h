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
  void ExecCmd(Connection& connection);

  string addressStr;
  int port_;
  fs::path images_path;
//  also serialize this vector
  std::vector<ProcessImage> processImages;
};

