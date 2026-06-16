cmake_minimum_required(VERSION 3.20)
project(Babel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this is the key line for clangd

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS} include/)
add_definitions(${LLVM_DEFINITIONS})

file(GLOB_RECURSE SRCS "src/*.cpp")

add_executable(babel ${SRCS})

llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    IRReader
    BitWriter
    Target
    MC
    DebugInfoDWARF
    DebugInfoCodeView
    Object
    Analysis
    Passes
    X86CodeGen
    X86AsmParser
    X86Desc
    X86Info
    AArch64CodeGen
    AArch64AsmParser
    AArch64Desc
    AArch64Info
)

target_link_libraries(babel PRIVATE ${LLVM_LIBS})

if(UNIX)
    target_link_options(babel PRIVATE ${LLVM_LD_FLAGS} -rdynamic)
endif()

# Install the executable
install(TARGETS babel
    RUNTIME DESTINATION .
)

# Install packaged files
install(DIRECTORY "${CMAKE_SOURCE_DIR}/PackagedFiles/"
    DESTINATION .
)