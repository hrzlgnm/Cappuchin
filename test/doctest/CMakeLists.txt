add_library(doctest INTERFACE)
add_library(doctest::doctest ALIAS doctest)

# HACK(hrzgnm): disable clang-tidy checks on doctest
target_include_directories(doctest SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(doctest_dll dll.cpp)
add_library(doctest::dll ALIAS doctest_dll)
target_link_libraries(doctest_dll PRIVATE doctest::doctest)
add_executable(doctest_exe main.cpp)

target_precompile_headers(doctest_exe REUSE_FROM cappuchin_lib)
target_link_libraries(
    doctest_exe
    PRIVATE
        doctest::dll
        doctest::doctest
        fmt::fmt
        cappuchin::lib
)
add_executable(doctest::exe ALIAS doctest_exe)
set_property(
    TARGET
        doctest_exe
    PROPERTY
        OUTPUT_NAME
            doctest
)
target_compile_features(doctest_exe PRIVATE cxx_std_20)

add_test(
    NAME doctest
    COMMAND
        $<TARGET_FILE:doctest_exe> --reporters=junit --out=junit.xml
)

# HACK(hrzgnm): disable analyzer checks on doctest targets
set_target_properties(
    doctest_exe
    PROPERTIES
        CXX_CPPCHECK
            ""
)
set_target_properties(
    doctest_dll
    PROPERTIES
        CXX_CPPCHECK
            ""
)
