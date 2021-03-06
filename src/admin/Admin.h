//
// Created by marcin on 27.05.17.
//

#ifndef PROMAN_ADMIN_H
#define PROMAN_ADMIN_H

#include <string>
#include <iostream>
#include <memory>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include "AdminServerConnection.h"

namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;
using std::cin;
using std::unique_ptr;
using std::make_unique;

namespace Adm {

class Admin {
public:
  void GetArguments(int argc, char** argv);
  bool IsInteractive();
  void BatchMode();
  bool CommandParser();
  void Connect();

private:
  string server_address_;
  int server_port_;
  po::variables_map vm_;
  unique_ptr<AdminServerConnection> connection_;

  void ParseCommand(string);
  void ListWorkers() const;
  void ListImages() const;
  void UploadImage(const string &imagePath) const;
  void ListWorkersImages() const;
  void UploadImageWorker(const vector<string> &elems) const;
  void DeleteImage(const vector<string> &elems) const;
  void DeleteImageWorker(const vector<string> &elems) const;
  void RunNow(const vector<string> &elems) const;
  void StopNow(const vector<string> &elems) const;
};

}

#endif //PROMAN_ADMIN_H
