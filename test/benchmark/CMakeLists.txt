project(Benchmark LANGUAGES CXX)

add_executable(benchmark source/main.cpp)

target_precompile_headers(benchmark REUSE_FROM cappuchin_lib)
target_link_libraries(
    benchmark
    PRIVATE
        fmt::fmt
        cappuchin::lib
)
target_compile_features(benchmark PRIVATE cxx_std_20)

add_folders(Benchmark)
