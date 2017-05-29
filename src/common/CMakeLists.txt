add_library(Connection Connection.h Connection.cpp)
add_library(ProcessImage ProcessImage.h ProcessImage.cpp)

target_link_libraries (ProcessImage boost_system)
target_link_libraries (ProcessImage boost_filesystem)
