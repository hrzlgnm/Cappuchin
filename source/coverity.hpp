
#ifdef COVERITY_BUILD
void __coverity_panic__()
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

#    undef REQUIRE
#    define REQUIRE(cond) ::coverity_require((cond))
#endif
