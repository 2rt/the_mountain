cmake_minimum_required(VERSION 4.1)
project(Project___20_ C)

set(CMAKE_C_STANDARD 11)

add_executable(Project___20_ gradient.c gradient_sol.c)
target_link_libraries(Project___20_ m)
