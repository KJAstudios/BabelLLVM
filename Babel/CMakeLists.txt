cmake_minimum_required(VERSION 3.20)
project(Babel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this is the key line for clangd

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS} include/)
add_definitions(${LLVM_DEFINITIONS})

file(GLOB_RECURSE SRCS "src/*.cpp")

add_executable(babel-llvm ${SRCS})

llvm_map_components_to_libnames(LLVM_LIBS core orcjit native)
target_link_libraries(babel-llvm ${LLVM_LIBS})
target_link_options(babel-llvm PRIVATE -rdynamic)