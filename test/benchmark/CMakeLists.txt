# Copyright 2023-2025 hrzlgnm
# SPDX-License-Identifier: MIT-0

project(Benchmark LANGUAGES CXX)

add_executable(benchmark source/main.cpp)

target_precompile_headers(benchmark REUSE_FROM cappuchin_lib)
target_link_libraries(
    benchmark
    PRIVATE
        fmt::fmt
        cappuchin::lib
)
# HACK(hrzlgnm): disable compiler warnings on fmt library,
# by forcing fmt as SYSTEM include
target_include_directories(benchmark SYSTEM PRIVATE $<TARGET_PROPERTY:fmt::fmt,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_features(benchmark PRIVATE cxx_std_20)

add_folders(Benchmark)
