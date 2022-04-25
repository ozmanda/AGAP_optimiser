cmake_minimum_required(VERSION 3.20)
project(optimiser)

set(CMAKE_CXX_STANDARD 14)

add_executable(optimiser src/main.cpp include/SA.cpp include/SA.h include/Archive.cpp include/Archive.h include/Epoch.cpp include/Epoch.h include/Solution.cpp include/Solution.h include/Flight.cpp include/Flight.h src/utils.cpp src/utils.h)
