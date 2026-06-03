# decree

A lightweight C++23 error-handling library built on `std::expected`. Functions return a typed result instead of throwing — the type system forces callers to handle both the success and the failure path.

```cpp
import errors;

enum class EFileError { NotFound, PermissionDenied };

Errors::ErrorResult<std::string, EFileError> readFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path))
        return Errors::makeError(EFileError::NotFound, path.string() + " does not exist");

    return readContent(path);
}

auto result = readFile("config.json");
if (result)
    process(*result);
else
    std::cerr << result.error().errorMessage << "\n";
```

## Build

Requires CMake 3.28+ and any C++23-capable compiler. Clang and GCC additionally require the Ninja generator.

```sh
cmake -G Ninja -B build -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

## Documentation

See [doc/error_handling.md](doc/error_handling.md) for the full API reference, architecture overview, and examples.
