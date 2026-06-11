cmake_minimum_required(VERSION 3.20)
project(Babel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this is the key line for clangd

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS} include/)
add_definitions(${LLVM_DEFINITIONS})

file(GLOB_RECURSE SRCS "src/*.cpp")

add_executable(babel-llvm ${SRCS})

# Let llvm-config determine exactly what needs to be linked
execute_process(
    COMMAND llvm-config-20 --libs all
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND llvm-config-20 --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND llvm-config-20 --ldflags
    OUTPUT_VARIABLE LLVM_LD_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_link_libraries(babel-llvm ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
target_link_options(babel-llvm PRIVATE ${LLVM_LD_FLAGS} -rdynamic)