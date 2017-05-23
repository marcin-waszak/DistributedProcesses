cmake_minimum_required (VERSION 3.7)
project (proman)

set (proman_VERSION_MAJOR 0)
set (proman_VERSION_MINOR 1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow")

# Executables
add_executable (admin "${PROJECT_SOURCE_DIR}/src/admin/Main.cpp")
add_executable (server "${PROJECT_SOURCE_DIR}/src/server/Main.cpp" src/server/Server.cpp src/server/Server.h)
target_link_libraries (admin AdminServerConnection)
target_link_libraries (admin Connection)
target_link_libraries (admin ProcessImage)
target_link_libraries (admin boost_program_options)

target_link_libraries (server Connection)
target_link_libraries (server ProcessImage)
target_link_libraries (server boost_program_options)

# TODO: server, worker
# TODO: move link_libraries somewhoere to subdirectory

add_subdirectory(src)
