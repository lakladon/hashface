cmake_minimum_required(VERSION 3.10)
project(hashface VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(ZLIB REQUIRED)

# Main executable
add_executable(hashface
    src/main.cpp
    src/avatar_generator.cpp
    src/md5.cpp
)

target_include_directories(hashface PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hashface PRIVATE
    ZLIB::ZLIB
)

# Install target
install(TARGETS hashface DESTINATION bin)