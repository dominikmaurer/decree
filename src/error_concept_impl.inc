template<typename T>
concept CError = requires (T error) {
    requires std::is_enum_v<decltype(error.errorCode)>;
    { error.errorMessage } -> std::convertible_to<std::string>;

    { T::makeError(error.errorCode, error.errorMessage) } -> std::same_as<T>;
    requires std::copyable<T>;
};
