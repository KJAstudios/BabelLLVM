cmake_minimum_required(VERSION 3.20)
project(Babel)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS} include/)
add_definitions(${LLVM_DEFINITIONS})

add_executable(babel-llvm
  src/main.cpp
  src/Lexer.cpp
  src/Parser.cpp
  src/AST.cpp
  src/Codegen.cpp
)

llvm_map_components_to_libnames(llvm_libs core support irreader)
target_link_libraries(babel-llvm ${llvm_libs})