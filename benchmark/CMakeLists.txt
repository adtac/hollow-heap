cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")

add_executable("all_tests" "all_tests.cpp")
target_compile_options("all_tests" PRIVATE "-Wno-write-strings")
target_link_libraries("all_tests")

add_executable("cuts" "cuts.cpp")
target_compile_options("cuts" PRIVATE "-Wno-write-strings")
target_link_libraries("cuts")

add_executable("roads" "roads.cpp")
target_compile_options("roads" PRIVATE "-Wno-write-strings")
target_link_libraries("roads")
