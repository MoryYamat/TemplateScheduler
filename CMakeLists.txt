cmake_minimum_required(VERSION 3.25)

project("TScheduler" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TShce src/TScheduler.cpp)