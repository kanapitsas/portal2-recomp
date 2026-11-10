// FUTEX_WAKE sur des adresses sans attendeur : 0 réveillé, aucune entrée créée.
#include "checks.h"

size_t rt_futex_count(void);   // test-only, runtime/futex.c

RT_CHECK(futex_wake_without_waiter_allocates_nothing) {
    CPU c; rt_cpu_reset(&c);
    size_t before = rt_futex_count();
    for (uint32_t i = 0; i < 64; ++i) {
        assert(rt_futex(&c, 0x2001f000u + 4 * i, 1, 1, 0) == 0);     // FUTEX_WAKE
        assert(rt_futex(&c, 0x2001f000u + 4 * i, 129, 8, 0) == 0);   // FUTEX_WAKE_PRIVATE
    }
    assert(rt_futex_count() == before);
}
