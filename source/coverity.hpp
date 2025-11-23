
#ifdef COVERITY_BUILD
extern "C" void __coverity_panic__(void);

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
