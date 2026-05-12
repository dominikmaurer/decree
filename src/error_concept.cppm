module;

#include <concepts>
#include <type_traits>
#include <string>

export module errorConcept;

export template<typename ErrorType>
concept CError = requires (ErrorType error, std::string errorMessage, typename ErrorType::EErrorCode errorCode) {
    { error.errorMessage } -> std::convertible_to<std::string>;

    typename ErrorType::EErrorCode;
    requires std::is_enum_v<typename ErrorType::EErrorCode>;
    { error.errorCode } -> std::same_as<typename ErrorType::EErrorCode&>;

    { ErrorType::makeError(errorCode, errorMessage) } -> std::same_as<ErrorType>;

    requires std::movable<ErrorType>;
    requires std::copyable<ErrorType>;
};
