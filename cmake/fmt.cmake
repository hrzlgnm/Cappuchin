include(FetchContent)

FetchContent_Declare(fmt GIT_REPOSITORY "https://github.com/fmtlib/fmt.git" GIT_TAG "12.1.0")

FetchContent_MakeAvailable(fmt)

# HACK(hrzlgnm): disable all warnings for fmt library
if(TARGET fmt)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(fmt PRIVATE -w)
    elseif(MSVC)
        target_compile_options(fmt PRIVATE /w)
    endif()
endif()

# HACK(hrzlgnm): disable analyzer checks on fmt library
set_target_properties(
    fmt
    PROPERTIES
        CXX_CLANG_TIDY
            ""
        CXX_CPPCHECK
            ""
)
