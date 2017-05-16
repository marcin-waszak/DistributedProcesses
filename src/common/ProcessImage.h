#pragma once

#include <string>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
class ProcessImage {
    fs::path file_path_;
    void check();

public:
    ProcessImage(const std::string& path);
    ProcessImage(const fs::path path);
    std::vector<char> GetBytes()const;
    fs::path GetPath() { return file_path_; }
};