cmake_minimum_required(VERSION 3.16)
project(WinFileRecovery LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WinFileRecovery
    src/main.cpp
    src/DiskScanner.cpp
    src/NTFSParser.cpp
    src/FileRecovery.cpp
    src/FileRepair.cpp
    src/Utils.cpp
)

target_include_directories(WinFileRecovery PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(WinFileRecovery PRIVATE shell32)
