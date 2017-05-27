//
// Created by marcin on 27.05.17.
//

#ifndef PROMAN_ADMIN_H
#define PROMAN_ADMIN_H

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;
using std::cin;

class Admin {
public:
  void GetArguments(int argc, char** argv);
  bool CommandParser();

private:
  string server_address_;
  int server_port_;
};


#endif //PROMAN_ADMIN_H
