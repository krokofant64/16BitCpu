cmake_minimum_required(VERSION 2.8)

# set the project name
project(cpu16)

if(MSVC)
   set(CMAKE_CXX_FLAGS "/EHsc")
else()
   set(CMAKE_CXX_FLAGS "-std=c++11")
endif(MSVC)


# add the executable
add_executable(cpu16 Cpu.cpp Memory.cpp Assembler.cpp main.cpp  OpcodeTable.cpp CpuControlPanel.cpp)


