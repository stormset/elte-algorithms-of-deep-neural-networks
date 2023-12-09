cmake_minimum_required(VERSION 3.19)
project(Islands)

set(CMAKE_CXX_STANDARD 14)

add_executable(Islands main.cpp CoverTree/CoverTree.h CoverTree/CoverTreePoint.h IslandsMap.h SampleData.h)