cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(rv_tlm_0)

find_package(SystemCLanguage CONFIG REQUIRED)



add_executable(rv_tlm_0 rv_tlm_0.cpp globals.h mem_rv.cpp mem_rv.h memory_content/code.bin memory_content/data.bin rv_tlm_0.h top.cpp)
target_link_libraries(rv_tlm_0 SystemC::systemc)