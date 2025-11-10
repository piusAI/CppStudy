//Static_ASSERT 11버전 이하
#include <iostream>

#ifdef __cplusplus
#if __cplusplus>=201103L
#define STATIC_ASSERT(expr) static_assert(expr, "static assert failed:" #expr)
#else
template<bool> class TStaticAssert;
template<> class TStaticAssert<true>{};

#define STATIC_ASSERT(expr)
enum
{
    ASSERT_GLUE(g_assert_fail_,__LINE__) = sizeof(TStaticAssert<!!(expr)>)
}
#endif
#endif

//C++ 2011이하면 다른거 활용


int main() {
    // Write C++ code here
    STATIC_ASSERT(1==1);
    STATIC_ASSERT(2==1);

    return 0;
}


