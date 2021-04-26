cmake_minimum_required(VERSION 3.17)
project(p2code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

add_executable(bl_server bl_server.c blather.h server_funcs.c util.c)
add_executable(bl_client bl_client.c blather.h server_funcs.c util.c simpio.c)
add_executable(bl_showlog bl_showlog.c blather.h util.c)
add_executable(simpio_demo simpio_demo.c blather.h simpio.c)

