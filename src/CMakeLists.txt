cmake_minimum_required(VERSION 3.12)
project(src)

set(CMAKE_CXX_STANDARD 11)

include_directories("../deps/linenoise")

include_directories("../deps/libelfin/dwarf")
include_directories("../deps/libelfin/elf")
include_directories("../deps/linenoise")

link_libraries("dwarf++")
link_libraries("elf++")
#set(CMAKE_CXX_FLAGS "-Wall -g -lelf++ -ldwarf++")
add_executable(edb main.cpp ../deps/linenoise/linenoise.c Debugger.cpp)