module;

#include <type_traits>
#include <string>
#include <expected>

export module errors;

export namespace Errors {
    template<typename EErrorCodeParam> requires std::is_enum_v<EErrorCodeParam>
    struct ErrorType {
        using EErrorCode = EErrorCodeParam;

        EErrorCode errorCode;
        std::string errorMessage;

        [[nodiscard]] static ErrorType makeError(EErrorCode code, std::string msg) {
            return { code, std::move(msg) };
        }
    };

    template<typename T, typename EErrorCode>
    using ErrorResult = std::expected<T, ErrorType<EErrorCode>>;
}