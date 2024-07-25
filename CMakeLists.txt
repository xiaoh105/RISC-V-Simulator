cmake_minimum_required(VERSION 3.29)
project(Simulator)

set(CMAKE_CXX_STANDARD 23)

include_directories(src/include)

add_executable(Judge src/main.cpp)
add_executable(Brute src/brute_main.cpp
        src/memory/memory.cpp
        src/memory/decoder.cpp
        src/reg_file/reg_file.cpp
        src/decoder/instruction_decoder.cpp
        src/alu/imm_gen.cpp
        src/alu/alu.cpp
        src/brute_cpu.cpp)
add_executable(Tomasulo src/tomasulo_main.cpp)