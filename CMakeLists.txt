cmake_minimum_required(VERSION 2.8)

# set the project name
project(cpu16)

set(CMAKE_CXX_FLAGS -std=c++11)

# add the executable
add_executable(cpu16 Cpu.cpp Memory.cpp Assembler.cpp main.cpp  OpcodeTable.cpp CpuControlPanel.cpp)


