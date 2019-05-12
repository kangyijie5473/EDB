cmake_minimum_required(VERSION 3.12)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories("${PROJECT_SOURCE_DIR}/deps/linenoise")

include_directories("${PROJECT_SOURCE_DIR}/deps/libelfin/dwarf")
include_directories("${PROJECT_SOURCE_DIR}/deps/libelfin/elf")
include_directories("${PROJECT_SOURCE_DIR}/deps/libunwind-1.4-rc1/include")
add_custom_target(
        libelfin
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/libelfin
)
#todo:update libunwind
add_custom_target(
        libunwind
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/libunwind-1.4-rc1
)
add_executable(edb src/main.cpp ${PROJECT_SOURCE_DIR}/deps/linenoise/linenoise.c src/Debugger.cpp src/Watcher.cpp src/Displayer.cpp)

target_link_libraries(edb
        ${PROJECT_SOURCE_DIR}/deps/libelfin/dwarf/libdwarf++.so
        ${PROJECT_SOURCE_DIR}/deps/libelfin/elf/libelf++.so
        ${PROJECT_SOURCE_DIR}/deps/libunwind-1.4-rc1/lib/libunwind.so
        ${PROJECT_SOURCE_DIR}/deps/libunwind-1.4-rc1/lib/libunwind-ptrace.so
        ${PROJECT_SOURCE_DIR}/deps/libunwind-1.4-rc1/lib/libunwind-x86_64.so
        )
add_dependencies(edb libelfin libunwind)

#set(CMAKE_CXX_FLAGS "-Wall -g  -lunwind-ptrace -lunwind -lunwind-x86_64")
