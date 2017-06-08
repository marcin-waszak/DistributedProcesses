#include "ProcessImage.h"

#include <fstream>
#include <iostream>


namespace bf = boost::filesystem;

static std::vector<char> ReadAllBytes(char const* filename)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    return result;
}

void ProcessImage::check() {
    if (!boost::filesystem::exists(file_path_)) {
        // TODO: proper error handling
        std::cout << "File \"" << file_path_ << "\" does not exist." << std::endl;
        exit(1);
    }
}

ProcessImage::ProcessImage(const std::string& path):file_path_(path) {
}

ProcessImage::ProcessImage(fs::path path):file_path_(path) {
}

std::vector<char> ProcessImage::GetBytes()const {
    if (!boost::filesystem::exists(file_path_)) {
        // TODO: proper error handling
        std::cout << "File \"" << file_path_ << "\" does not exist." << std::endl;
        exit(1);
    }
    return ReadAllBytes(file_path_.string().data());
}
