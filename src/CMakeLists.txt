cmake_minimum_required(VERSION 3.12)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories("../deps/linenoise")

include_directories("../deps/libelfin/dwarf")
include_directories("../deps/libelfin/elf")
include_directories("../deps/linenoise")
include_directories("/home/kang/下载/libunwind-1.4-rc1/include")

link_libraries("dwarf++")
link_libraries("elf++")
set(CMAKE_CXX_FLAGS "-Wall -g -lunwind-ptrace -lunwind -lunwind-x86_64")
add_executable(edb main.cpp ../deps/linenoise/linenoise.c Debugger.cpp)
add_executable(test testunwind.cpp )
