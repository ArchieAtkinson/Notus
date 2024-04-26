#pragma once


#include <zephyr/ztest.h>


// ztest_assert.h
#undef zassert_ok
#define zassert_ok(cond, ...) zassert(!(cond), #cond " is non-zero", ##__VA_ARGS__) // NOLINT

#undef zassert_not_ok
#define zassert_not_ok(cond, ...) zassert(!!(cond), #cond " is zero", ##__VA_ARGS__) // NOLINT

#undef zassert_unreachable
#define zassert_unreachable(...) zassert(0, "Reached unreachable code", ##__VA_ARGS__) // NOLINT

#undef zassert_true
#define zassert_true(cond, ...) zassert(cond, #cond " is false", ##__VA_ARGS__) // NOLINT

#undef zassert_equal
#define zassert_equal(a, b, ...) zassert((a) == (b), #a " not equal to " #b, ##__VA_ARGS__) // NOLINT

#undef zassert_within
#define zassert_within(a, b, d, ...) zassert(((a) >= ((b) - (d))) && ((a) <= ((b) + (d))), #a " not within " #b " +/- " #d, ##__VA_ARGS__) // NOLINT

// ztest_test_new.h
#undef ZTEST
#define ZTEST(suite, fn) Z_ZTEST(suite, fn, 0) // NOLINT

