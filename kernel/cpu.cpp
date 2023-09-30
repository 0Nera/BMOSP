#include <stdint.h>
#include <stdbool.h>


namespace cpu {
    static bool x87_support = false;
    static bool sse_support = false;
    static bool avx_support = false;

    void init() {
        x87_support = false;
        sse_support = false;
        avx_support = false;
    }
}
