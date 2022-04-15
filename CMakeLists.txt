cmake_minimum_required(VERSION 3.0)
project(rv_tlm_0)

find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_executable(rv_tlm_0 rv_tlm_0.cpp)
target_link_libraries(rv_tlm_0 SystemC::systemc)