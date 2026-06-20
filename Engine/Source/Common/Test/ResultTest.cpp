//
// Created by johnk on 2026/6/20.
//

#include <string>
#include <memory>

#include <Test/Test.h>

#include <Common/Result.h>
using namespace Common;

TEST(ResultTest, OkTest)
{
    const Result<int, std::string> result = Ok(42);
    ASSERT_TRUE(result.IsOk());
    ASSERT_FALSE(result.IsErr());
    ASSERT_TRUE(static_cast<bool>(result));
    ASSERT_EQ(result.Value(), 42);
}

TEST(ResultTest, ErrTest)
{
    const Result<int, std::string> result = Err(std::string("bad"));
    ASSERT_FALSE(result.IsOk());
    ASSERT_TRUE(result.IsErr());
    ASSERT_FALSE(static_cast<bool>(result));
    ASSERT_EQ(result.Error(), "bad");
}

TEST(ResultTest, UnwrapTest)
{
    const Result<int, std::string> ok = Ok(42);
    ASSERT_EQ(ok.Unwrap(), 42);

    const Result<int, std::string> err = Err(std::string("bad"));
    ASSERT_EQ(err.UnwrapErr(), "bad");
}

TEST(ResultTest, ExpectTest)
{
    const Result<int, std::string> ok = Ok(42);
    ASSERT_EQ(ok.Expect("must be ok"), 42);

    const Result<int, std::string> err = Err(std::string("bad"));
    ASSERT_EQ(err.ExpectErr("must be err"), "bad");
}

TEST(ResultTest, UnwrapOrTest)
{
    const Result<int, std::string> ok = Ok(1);
    ASSERT_EQ(ok.UnwrapOr(7), 1);

    const Result<int, std::string> err = Err(std::string("bad"));
    ASSERT_EQ(err.UnwrapOr(7), 7);
    ASSERT_EQ(err.UnwrapOrElse([](const std::string& e) -> int { return static_cast<int>(e.size()); }), 3);
}

TEST(ResultTest, MapTest)
{
    const Result<int, std::string> ok = Ok(21);
    const Result<int, std::string> mapped = ok.Map([](const int& v) -> int { return v * 2; });
    ASSERT_TRUE(mapped.IsOk());
    ASSERT_EQ(mapped.Value(), 42);

    const Result<int, std::string> err = Err(std::string("bad"));
    const Result<int, std::string> mappedErr = err.Map([](const int& v) -> int { return v * 2; });
    ASSERT_TRUE(mappedErr.IsErr());
    ASSERT_EQ(mappedErr.Error(), "bad");
}

TEST(ResultTest, MapErrTest)
{
    const Result<int, std::string> err = Err(std::string("bad"));
    const Result<int, size_t> mapped = err.MapErr([](const std::string& e) -> size_t { return e.size(); });
    ASSERT_TRUE(mapped.IsErr());
    ASSERT_EQ(mapped.Error(), 3);
}

TEST(ResultTest, AndThenTest)
{
    const Result<int, std::string> ok = Ok(4);
    const Result<int, std::string> chained = ok.AndThen([](const int& v) -> Result<int, std::string> {
        return v > 0 ? Result<int, std::string>(Ok(v * 10)) : Result<int, std::string>(Err(std::string("non positive")));
    });
    ASSERT_TRUE(chained.IsOk());
    ASSERT_EQ(chained.Value(), 40);
}

TEST(ResultTest, OrElseTest)
{
    const Result<int, std::string> err = Err(std::string("bad"));
    const Result<int, int> recovered = err.OrElse([](const std::string& e) -> Result<int, int> {
        return Err(static_cast<int>(e.size()));
    });
    ASSERT_TRUE(recovered.IsErr());
    ASSERT_EQ(recovered.Error(), 3);
}

TEST(ResultTest, ToOptionalTest)
{
    const Result<int, std::string> ok = Ok(42);
    ASSERT_EQ(ok.ToOptional(), std::optional<int>(42));

    const Result<int, std::string> err = Err(std::string("bad"));
    ASSERT_FALSE(err.ToOptional().has_value());
}

TEST(ResultTest, VoidResultTest)
{
    const Result<void, std::string> ok = Ok();
    ASSERT_TRUE(ok.IsOk());

    const Result<void, std::string> err = Err(std::string("bad"));
    ASSERT_TRUE(err.IsErr());
    ASSERT_EQ(err.Error(), "bad");

    const Result<void, size_t> mapped = err.MapErr([](const std::string& e) -> size_t { return e.size(); });
    ASSERT_TRUE(mapped.IsErr());
    ASSERT_EQ(mapped.Error(), 3);
}

TEST(ResultTest, MoveOnlyValueTest)
{
    Result<std::unique_ptr<int>, std::string> result = Ok(std::make_unique<int>(5));
    ASSERT_TRUE(result.IsOk());

    const std::unique_ptr<int> value = std::move(result).Unwrap();
    ASSERT_EQ(*value, 5);
}

TEST(ResultTest, SameValueAndErrorTypeTest)
{
    const Result<std::string, std::string> ok = Ok(std::string("value"));
    ASSERT_TRUE(ok.IsOk());
    ASSERT_EQ(ok.Value(), "value");

    const Result<std::string, std::string> err = Err(std::string("error"));
    ASSERT_TRUE(err.IsErr());
    ASSERT_EQ(err.Error(), "error");
}
