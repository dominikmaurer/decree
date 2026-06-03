template<typename ErrorCodeEnum> requires std::is_enum_v<ErrorCodeEnum>
class ErrorType {
public:
    using EErrorCode = ErrorCodeEnum;

    EErrorCode  errorCode;
    std::string errorMessage;

    [[nodiscard]] static ErrorType makeError(const EErrorCode dErrorCode, std::string dErrorMessage) {
        return ErrorType(dErrorCode, std::move(dErrorMessage));
    }

private:
    ErrorType(const EErrorCode dErrorCode, std::string dErrorMessage) :
        errorCode(dErrorCode),
        errorMessage(std::move(dErrorMessage)) {
        // Nothing to do here
    }
};

template<typename T, typename ErrorCodeEnum>
using ErrorResult = std::expected<T, ErrorType<ErrorCodeEnum>>;

template<typename ErrorCodeEnum>
[[nodiscard]] std::unexpected<ErrorType<ErrorCodeEnum>> makeError(ErrorCodeEnum dErrorCode, std::string dErrorMessage) {
    return std::unexpected(ErrorType<ErrorCodeEnum>::makeError(dErrorCode, std::move(dErrorMessage)));
}
