enum class EFileError {
    eNotFound,
    ePermissionDenied
};

enum class EParseError {
    eInvalidFormat
};

TEST(ErrorTypeTest, MakeErrorSetsCode) {
    auto err = Decree::ErrorType<EFileError>::makeError(EFileError::eNotFound, "not found");

    EXPECT_EQ(err.getErrorCode(), EFileError::eNotFound);
}

TEST(ErrorTypeTest, MakeErrorSetsMessage) {
    auto err = Decree::ErrorType<EFileError>::makeError(EFileError::ePermissionDenied, "access denied");

    EXPECT_EQ(err.getErrorMessage(), "access denied");
}

TEST(ErrorTypeTest, MakeErrorCapturesSourceLocation) {
    const auto expectedLine = static_cast<uint_least32_t>(__LINE__ + 1);
    auto err = Decree::ErrorType<EFileError>::makeError(EFileError::eNotFound, "not found");

    EXPECT_EQ(err.getSourceLocation().line(), expectedLine);
    EXPECT_NE(std::string_view(err.getSourceLocation().file_name()).find("decree_tests_impl"), std::string_view::npos);
}

TEST(ErrorTypeTest, FreeFunctionMakeErrorCapturesSourceLocation) {
    const auto expectedLine = static_cast<uint_least32_t>(__LINE__ + 1);
    auto result = Decree::makeError(EFileError::eNotFound, "not found");

    EXPECT_EQ(result.error().getSourceLocation().line(), expectedLine);
    EXPECT_NE(std::string_view(result.error().getSourceLocation().file_name()).find("decree_tests_impl"), std::string_view::npos);
}

TEST(ErrorTypeTest, IsCopyConstructible) {
    auto original = Decree::ErrorType<EFileError>::makeError(EFileError::eNotFound, "msg");
    auto copy = original;

    EXPECT_EQ(copy.getErrorCode(), original.getErrorCode());
    EXPECT_EQ(copy.getErrorMessage(), original.getErrorMessage());
    EXPECT_EQ(copy.getSourceLocation().line(), original.getSourceLocation().line());
}

TEST(ErrorTypeTest, IsMoveConstructible) {
    auto err = Decree::ErrorType<EFileError>::makeError(EFileError::eNotFound, "msg");
    const auto expectedLine = err.getSourceLocation().line();
    auto moved = std::move(err);

    EXPECT_EQ(moved.getErrorCode(), EFileError::eNotFound);
    EXPECT_EQ(moved.getErrorMessage(), "msg");
    EXPECT_EQ(moved.getSourceLocation().line(), expectedLine);
}

static Decree::ErrorResult<int, EParseError> parsePositive(int value) {
    if(value <= 0) {
        return Decree::makeError(EParseError::eInvalidFormat, "must be positive");
    }

    return value;
}

static Decree::ErrorResult<void, EFileError> openFile(bool exists) {
    if(!exists) {
        return Decree::makeError(EFileError::eNotFound, "file not found");
    }

    return {};
}

static Decree::ErrorResult<std::string, EFileError> loadContent(bool fileExists) {
    auto result = openFile(fileExists);
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }
    return "content";
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
    EXPECT_EQ(result.error().getErrorCode(), EParseError::eInvalidFormat);
    EXPECT_EQ(result.error().getErrorMessage(), "must be positive");
}

TEST(ErrorResultTest, VoidSuccessPathHasValue) {
    auto result = openFile(true);

    ASSERT_TRUE(result.has_value());
}

TEST(ErrorResultTest, VoidFailurePathHasError) {
    auto result = openFile(false);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().getErrorCode(), EFileError::eNotFound);
    EXPECT_EQ(result.error().getErrorMessage(), "file not found");
}

TEST(ErrorResultTest, SourceLocationPreservedOnPropagation) {
    const auto original   = openFile(false);
    const auto propagated = loadContent(false);

    ASSERT_FALSE(propagated.has_value());
    EXPECT_EQ(propagated.error().getSourceLocation().line(), original.error().getSourceLocation().line());
    EXPECT_STREQ(propagated.error().getSourceLocation().file_name(), original.error().getSourceLocation().file_name());
}

TEST(ErrorResultTest, AndThenChainsPropagatesValue) {
    static constexpr auto s_input = 10;
    static constexpr auto s_multiplier = 2;
    static constexpr auto s_expected = (s_input * s_multiplier);
    auto result = parsePositive(s_input).and_then([](int v) -> Decree::ErrorResult<int, EParseError> {
        return v * s_multiplier;
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, s_expected);
}

TEST(ErrorResultTest, AndThenShortCircuitsOnError) {
    static constexpr auto s_invalidInput = -1;
    static constexpr auto s_fallbackValue = 0;
    bool chainCalled = false;
    auto result = parsePositive(s_invalidInput).and_then([&](int) -> Decree::ErrorResult<int, EParseError> {
        chainCalled = true;

        return s_fallbackValue;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(chainCalled);
}

TEST(ErrorResultTest, OrElsePropagatesRecoveredValue) {
    static constexpr auto s_invalidInput = -1;
    static constexpr auto s_recoveryValue = 0;
    auto result = parsePositive(s_invalidInput).or_else([](const Decree::ErrorType<EParseError>&) -> Decree::ErrorResult<int, EParseError> {
        return s_recoveryValue;
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, s_recoveryValue);
}

TEST(ErrorResultTest, OrElseForwardsError) {
    static constexpr auto s_invalidInput = -1;
    auto result = parsePositive(s_invalidInput).or_else([](const Decree::ErrorType<EParseError>& err) -> Decree::ErrorResult<int, EParseError> {
        return std::unexpected(err);
    });

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().getErrorCode(), EParseError::eInvalidFormat);
}

