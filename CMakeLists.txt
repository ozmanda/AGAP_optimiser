cmake_minimum_required(VERSION 3.20)
project(optimiser)

set(CMAKE_CXX_STANDARD 14)

add_executable(optimiser src/main.cpp include/optimiser/SA.cpp include/optimiser/SA.h include/optimiser/Archive.cpp include/optimiser/Archive.h include/optimiser/Epoch.cpp include/optimiser/Epoch.h include/optimiser/Solution.cpp include/optimiser/Solution.h include/optimiser/Flight.cpp include/optimiser/Flight.h)
