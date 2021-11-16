#define POLY_TEST_INSTRUMENTATION

#include <gtest/gtest.h>

#include "poly/function.hpp"

#include <array>

int fn_ptr(int a, int b) {
    return a+b;
}

TEST(PolyFunction, Lambda)
{
    poly::function<int()> fn([n=0]() mutable {
        return n++;
    });
    EXPECT_EQ(fn(), 0);
    EXPECT_EQ(fn(), 1);
    EXPECT_EQ(fn(), 2);

    poly::function<int()> fn2 = fn;

    EXPECT_EQ(fn(), 3);
    EXPECT_EQ(fn2(), 3);

    fn2 = []() {
        return 20;
    };

    EXPECT_EQ(fn2(), 20);
}

TEST(PolyFunction, FnPtr)
{
    poly::function<int(int, int)> fn(fn_ptr);
    EXPECT_EQ(fn(1, 2), 3);
    EXPECT_EQ(fn(4, 5), 9);
}

template<class Tag>
struct tracker
{
    static int move_construct;
    static int move_assign;
    static int copy_construct;
    static int copy_assign;

    tracker() = default;
    tracker(const tracker&)
    {
        copy_construct++;
    }
    tracker& operator=(const tracker&)
    {
        copy_assign++;
        return *this;
    }

    tracker(tracker&&) noexcept
    {
        move_construct++;
    }

    tracker& operator=(tracker&&) noexcept {
        move_assign++;
        return *this;
    }

    void operator()() {

    }
};

template<class T>
int tracker<T>::move_construct = 0;

template<class T>
int tracker<T>::copy_construct = 0;

template<class T>
int tracker<T>::move_assign = 0;

template<class T>
int tracker<T>::copy_assign = 0;

TEST(PolyFunction, CopyConstruct)
{
    using tracker_t = tracker<struct CopyConstruct>;
    tracker_t tracker;
    poly::function<void()> fn(tracker);
    EXPECT_GE(tracker_t::copy_construct, 1);
    EXPECT_EQ(tracker_t::move_construct, 0);
    int current = tracker_t::copy_construct;
    poly::function<void()> fn2 = fn;
    EXPECT_EQ(tracker_t::copy_construct, current+1);
    EXPECT_EQ(tracker_t::move_construct, 0);
    current++;
    fn = fn2;
    EXPECT_EQ(tracker_t::copy_construct, current+1);
    EXPECT_EQ(tracker_t::move_construct, 0);

    // No change expected!
    fn = fn;
    EXPECT_EQ(tracker_t::copy_construct, current+1);
    EXPECT_EQ(tracker_t::move_construct, 0);
}

TEST(PolyFunction, SmallToLargeCopyConstruct)
{
    using tracker_t = tracker<struct SmallToLargeCopyConstruct>;
    tracker_t tracker;
    poly::basic_function<void(), 8> fn(tracker);
    EXPECT_GE(tracker_t::copy_construct, 1);
    EXPECT_EQ(tracker_t::move_construct, 0);
    int current = tracker_t::copy_construct;

    poly::function<void()> fn2 = fn;
    EXPECT_EQ(tracker_t::copy_construct, current+1);
    EXPECT_EQ(tracker_t::move_construct, 0);
}

TEST(PolyFunction, MoveConstruct)
{
    using tracker_t = tracker<struct MoveConstruct>;
    tracker_t tracker;
    poly::function<void()> fn(poly::move(tracker));
    EXPECT_GE(tracker_t::copy_construct, 0);
    EXPECT_EQ(tracker_t::move_construct, 1);
    int current = tracker_t::move_construct;
    poly::function<void()> fn2 = poly::move(fn);
    EXPECT_EQ(tracker_t::move_construct, current+1);
    EXPECT_EQ(tracker_t::copy_construct, 0);
    current++;
    fn = poly::move(fn2);
    EXPECT_EQ(tracker_t::move_construct, current+1);
    EXPECT_EQ(tracker_t::copy_construct, 0);

    // No change expected!
    fn = poly::move(fn);
    EXPECT_EQ(tracker_t::move_construct, current+1);
    EXPECT_EQ(tracker_t::copy_construct, 0);
}

TEST(PolyFunction, SmallToLargeMoveConstruct)
{
    using tracker_t = tracker<struct SmallToLargeMoveConstruct>;
    tracker_t tracker;
    poly::basic_function<void(), 8> fn(poly::move(tracker));
    EXPECT_GE(tracker_t::copy_construct, 0);
    EXPECT_EQ(tracker_t::move_construct, 1);
    int current = tracker_t::move_construct;
    poly::function<void()> fn2 = poly::move(fn);
    EXPECT_EQ(tracker_t::move_construct, current+1);
    EXPECT_EQ(tracker_t::copy_construct, 0);
}

TEST(PolyFunction, MaxStorage)
{
    struct callable
    {
        std::array<uint8_t, 16> array_;
        size_t operator()() {
            return array_.size();
        }
    };

    using function_t = poly::basic_function<size_t(), 16>;
    using function2_t = poly::basic_function<size_t(), 17>;
    ASSERT_EQ(sizeof(callable), 16);
    ASSERT_GT(sizeof(function_t), 16);

    function_t fn(callable{});
    EXPECT_EQ(fn(), 16);
    function2_t fn2 = fn;
    EXPECT_EQ(fn2(), 16);

    // Make sure we store the callable and not poly::basic_function
    EXPECT_EQ(fn2.size(), sizeof(callable));
    fn2 = poly::move(fn);
    EXPECT_EQ(fn2(), 16);
    EXPECT_EQ(fn2.size(), sizeof(callable));
}

TEST(PolyFunction, VoidDefault)
{
    // Compile test only
    poly::function<void(int)> default_c;
    default_c(0);
}

TEST(PolyFunction, MemFn)
{
    struct test_struct
    {
        int value = 10;
        int fn(int a) {
            return 2*a;
        }
    };

    auto mem_fn = poly::mem_fn(&test_struct::value);
    test_struct t;
    EXPECT_EQ(mem_fn(t), 10);
    EXPECT_EQ(mem_fn(&t), 10);

    t.value = 20;
    EXPECT_EQ(mem_fn(t), 20);
    EXPECT_EQ(mem_fn(&t), 20);
}

