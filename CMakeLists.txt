cmake_minimum_required (VERSION 3.7)
project (proman)

set (proman_VERSION_MAJOR 0)
set (proman_VERSION_MINOR 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Executables
add_executable (admin "${PROJECT_SOURCE_DIR}/src/admin/main.cpp")
target_link_libraries(admin AdminServerConnection)
target_link_libraries (admin boost_program_options)

# TODO: server, worker
# TODO: move link_libraries somewhoere to subdirectory

add_subdirectory(src)
