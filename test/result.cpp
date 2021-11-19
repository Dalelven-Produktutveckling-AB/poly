#include <gtest/gtest.h>

#include <poly/result.hpp>

#include <string>

TEST(Result, ImplicitVoidResult)
{
    poly::result<int, std::string> ok_value = poly::ok(1);
    poly::result<void, std::string> as_void = poly::ok();
    as_void = ok_value;

    EXPECT_TRUE(as_void.is_ok());

    poly::result<int, std::string> error = poly::error("Some error");
    as_void = error;
    ASSERT_TRUE(as_void.is_error());
    EXPECT_EQ(as_void.unwrap_error(), "Some error");
}
