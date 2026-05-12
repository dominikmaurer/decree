import errors;
import errorConcept;

#include <gtest/gtest.h>
#include <string>
#include <expected>

enum class EFileError  { 
    NotFound, 
    PermissionDenied 
};

enum class EParseError { 
    InvalidFormat, 
    UnexpectedEof 
};

TEST(ErrorTypeTest, MakeErrorSetsCode) {
    auto err = Errors::ErrorType<EFileError>::makeError(EFileError::NotFound, "not found");

    EXPECT_EQ(err.errorCode, EFileError::NotFound);
}

TEST(ErrorTypeTest, MakeErrorSetsMessage) {
    auto err = Errors::ErrorType<EFileError>::makeError(EFileError::PermissionDenied, "access denied");

    EXPECT_EQ(err.errorMessage, "access denied");
}

TEST(ErrorTypeTest, IsCopyConstructible) {
    auto original = Errors::ErrorType<EFileError>::makeError(EFileError::NotFound, "msg");
    auto copy = original;

    EXPECT_EQ(copy.errorCode, original.errorCode);
    EXPECT_EQ(copy.errorMessage, original.errorMessage);
}

TEST(ErrorTypeTest, IsMoveConstructible) {
    auto err  = Errors::ErrorType<EFileError>::makeError(EFileError::NotFound, "msg");
    auto moved = std::move(err);

    EXPECT_EQ(moved.errorCode, EFileError::NotFound);
    EXPECT_EQ(moved.errorMessage, "msg");
}

static Errors::ErrorResult<int, EParseError> parsePositive(int value) {
    if (value <= 0) {
        return std::unexpected(Errors::ErrorType<EParseError>::makeError(EParseError::InvalidFormat, "must be positive"));
    }
        
    return value;
}

TEST(ErrorResultTest, SuccessPathHasValue) {
    static constexpr auto s_input = 42;
    auto result = parsePositive(s_input);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, s_input);
}

TEST(ErrorResultTest, FailurePathHasError) {
    static constexpr auto s_invalidInput = -1;
    auto result = parsePositive(s_invalidInput);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().errorCode, EParseError::InvalidFormat);
    EXPECT_EQ(result.error().errorMessage, "must be positive");
}

TEST(ErrorResultTest, AndThenChainsPropagatesValue) {
    static constexpr auto s_input = 10;
    static constexpr auto s_multiplier = 2;
    static constexpr auto s_expected = (s_input * s_multiplier);
    auto result = parsePositive(s_input).and_then([](int v) -> Errors::ErrorResult<int, EParseError> { 
        return v * s_multiplier; 
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, s_expected);
}

TEST(ErrorResultTest, AndThenShortCircuitsOnError) {
    static constexpr auto s_invalidInput = -1;
    static constexpr auto s_fallbackValue = 0;
    bool chainCalled = false;
    auto result = parsePositive(s_invalidInput).and_then([&](int) -> Errors::ErrorResult<int, EParseError> {
        chainCalled = true;

        return s_fallbackValue;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(chainCalled);
}

TEST(ErrorResultTest, OrElsePropagatesRecoveredValue) {
    static constexpr auto s_invalidInput = -1;
    static constexpr auto s_recoveryValue = 0;
    auto result = parsePositive(s_invalidInput).or_else([](const Errors::ErrorType<EParseError>&) -> Errors::ErrorResult<int, EParseError> {
        return s_recoveryValue;
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, s_recoveryValue);
}

TEST(CErrorConceptTest, ErrorTypeSatisfiesCError) {
    static_assert(CError<Errors::ErrorType<EFileError>>);
    static_assert(CError<Errors::ErrorType<EParseError>>);
    SUCCEED();
}

TEST(CErrorConceptTest, PlainStructDoesNotSatisfyCError) {
    struct NoCode { std::string errorMessage; };
    static_assert(!CError<NoCode>);
    SUCCEED();
}

TEST(CErrorConceptTest, IntDoesNotSatisfyCError) {
    static_assert(!CError<int>);
    SUCCEED();
}

struct CustomError {
    enum class EErrorCode { 
        Ok,
        Overflow, 
        Underflow 
    };

    std::string errorMessage;
    EErrorCode  errorCode;

    static CustomError makeError(EErrorCode code, std::string msg) {
        return { std::move(msg), code };
    }
};

TEST(CustomErrorTypeTest, SatisfiesCError) {
    static_assert(CError<CustomError>);
    SUCCEED();
}

TEST(CustomErrorTypeTest, MakeErrorRoundTrip) {
    auto err = CustomError::makeError(CustomError::EErrorCode::Overflow, "overflow detected");

    EXPECT_EQ(err.errorCode,    CustomError::EErrorCode::Overflow);
    EXPECT_EQ(err.errorMessage, "overflow detected");
}

TEST(CustomErrorTypeTest, UsableAsErrorResultErrorType) {
    static constexpr auto s_successValue = 1;
    using Result = std::expected<int, CustomError>;
    Result ok  = s_successValue;
    Result bad = std::unexpected(CustomError::makeError(CustomError::EErrorCode::Underflow, "underflow"));

    EXPECT_TRUE(ok.has_value());
    EXPECT_FALSE(bad.has_value());
    EXPECT_EQ(bad.error().errorCode, CustomError::EErrorCode::Underflow);
}
