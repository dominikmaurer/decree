template<typename ErrorCodeEnum> requires std::is_enum_v<ErrorCodeEnum>
class ErrorType {
public:
    using EErrorCode = ErrorCodeEnum;

    [[nodiscard]] EErrorCode getErrorCode() const {
        return m_errorCode;
    }

    [[nodiscard]] std::string_view getErrorMessage() const {
        return m_errorMessage;
    }

    [[nodiscard]] std::source_location getSourceLocation() const {
        return m_sourceLocation;
    }

    [[nodiscard]] static ErrorType makeError(const EErrorCode errorCode, std::string_view errorMessage, std::source_location location = std::source_location::current()) {
        return ErrorType(errorCode, errorMessage, location);
    }

private:
    ErrorType(const EErrorCode errorCode, std::string_view errorMessage, std::source_location location) :
        m_errorCode(errorCode),
        m_errorMessage(errorMessage),
        m_sourceLocation(location) {
        // Nothing to do here
    }

    EErrorCode m_errorCode;
    std::string m_errorMessage;
    std::source_location m_sourceLocation;
};

template<typename T, typename ErrorCodeEnum> requires std::is_enum_v<ErrorCodeEnum>
using ErrorResult = std::expected<T, ErrorType<ErrorCodeEnum>>;

template<typename ErrorCodeEnum> requires std::is_enum_v<ErrorCodeEnum>
[[nodiscard]] std::unexpected<ErrorType<ErrorCodeEnum>> makeError(ErrorCodeEnum errorCode, std::string_view errorMessage, std::source_location location = std::source_location::current()) {
    return std::unexpected(ErrorType<ErrorCodeEnum>::makeError(errorCode, errorMessage, location));
}
