
#ifdef COVERITY_BUILD
static inline void __coverity_panic__()
{
    abort();
}

template<typename T>
static inline void coverity_require(const T& t)
{
    if (!t) {
        __coverity_panic__();
    }
}

#    define DOCTEST_REQUIRE_ORIG REQUIRE

#    define REQUIRE(...) \
        do { \
            coverity_require((__VA_ARGS__)); \
            DOCTEST_REQUIRE_ORIG(__VA_ARGS__); \
        } while (0)

#endif
