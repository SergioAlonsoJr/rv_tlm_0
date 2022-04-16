cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11")

project(rv_tlm_0)

find_package(SystemCLanguage CONFIG REQUIRED)

#set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_executable(rv_tlm_0 rv_tlm_0.cpp riscV.cpp globals.h memoria.cpp memoria.h memory_content/code.bin memory_content/data.bin)
target_link_libraries(rv_tlm_0 SystemC::systemc)