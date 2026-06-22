# Coding Guidelines

C++23, Clang 17+, Ninja. Formatting is enforced by `.clang-format` — run `clang-format -i` before committing.

---

## Tabs vs. Spaces — The Great Silence

Somewhere in the world, two developers are arguing about tabs versus spaces right now. One has written a 47-page manifesto. The other has opened a GitHub issue. Neither has seen their family in weeks.

We don't do that here.

The rule is simple: **4 spaces per indentation level.** If you use tabs, configure your editor to display and save them as 4 spaces. The result is identical. No discussion needed. No one has to be right. `.clang-format` reformats everything anyway — the only opinion that matters is the compiler's, and it doesn't care.

```cpp
void example() {
    if(condition) {       // 4 spaces. Done. No meeting required.
        doSomething();
    }
}
```

---

## Naming

| Element                     | Convention            | Example               |
| --------------------------- | --------------------- | --------------------- |
| File                        | `snake_case`          | `time_helpers.cppm`   |
| Module                      | `camelCase`           | `timeHelpers`         |
| Class / struct              | `PascalCase`          | `TimeHelpers`         |
| Namespace                   | `PascalCase`          | `FileHelpers`         |
| Enum type                   | `E` + `PascalCase`    | `EError`              |
| Enum value                  | `e` + `camelCase`     | `eNoError`            |
| Member variable             | `m_` + `camelCase`    | `m_pinNumber`         |
| Pointer member variable     | `m_p` + `camelCase`   | `m_pImpl`             |
| Static / constexpr constant | `s_` + `camelCase`    | `s_byteMask`          |
| Template parameter          | `PascalCase`          | `RepresentationType`  |
| Function / parameter / local| `camelCase`           | `isFunctionTrue`      |

---

## Modules

Every translation unit uses the C++23 module format:

```cpp
module;                  // global module fragment — #includes only

#include <chrono>

export module myModule;  // module declaration

export class MyClass {}; // exported API
```

Implementation units (`.cpp`) open with `module myModule;` and contain no `export` declarations.

---

## Namespaces

Namespace contents are indented. No closing comment.

```cpp
export namespace FileHelpers {
    void appendToCsv(const std::string filename, const std::string content) {
        ...
    }
}
```

Use an anonymous namespace in `.cpp` implementation units to give file-scoped constants and helpers internal linkage instead of `static`:

```cpp
module myModule;

namespace {
    constexpr auto s_defaultTimeout = 1000;
}
```

---

## Types and Qualifiers

- Prefer `auto` for local variables when the type is apparent from context.
- Use `const` on all variables and parameters that are not modified.
- Use `static_cast` for all explicit conversions — never C-style casts.
- Use `[[nodiscard]]` on any function whose return value the caller must check.
- Use `consteval` for functions that must be evaluated at compile time.
- Pointers and references bind to the type, not the variable name: `T*`, `T&`, `T&&`.

```cpp
void process(const std::string& input);
EError busyWaitWithTimeout(WaitFunction&& fn, ...);
```

---

## Control Flow

No space between control flow keywords and the opening parenthesis.

Always use braces — even for single-statement bodies. The opening brace is always on the same line as the keyword.

```cpp
if(condition) {
    ...
}

if(condition) {
    ...
} else {
    ...
}

if(condition1) {
    ...
} else if(condition2) {
    ...
} else {
    ...
}

while(condition) {
    ...
}
for(int i = 0; i < n; ++i) {
    ...
}
switch(value) {
    ...
}
```

Wrap complex subexpressions in comparisons with parentheses for clarity:

```cpp
if((std::chrono::steady_clock::now() - start) >= timeout) { ... }
```

Always leave an empty line before a `return` statement in non-trivial functions:

```cpp
void compute() {
    const auto result = doWork();

    return result;
}
```

---

## Enums

Always use `enum class` — never plain `enum`. Enum values are always on separate lines, never collapsed to a single line.

Name the type with the `E` prefix and each value with the `e` prefix, as listed in the Naming table.

```cpp
enum class EError {
    eNoError,
    eTimeouted
};
```

Do not assign explicit integer values unless interoperating with an external protocol or hardware register that mandates specific values.

---

## Classes

- `public` / `private` / `protected` are not indented relative to the class body.
- Member variables are declared `private` and follow the `m_` prefix.
- Static compile-time constants follow the `s_` prefix.
- Prefer in-class member initialisation over constructor initialiser lists for simple defaults.

```cpp
class MyClass {
public:
    void doSomething();

private:
    int m_value = 0;
    bool m_isReady = false;
    static constexpr int s_limit = 100;
};
```

**Special member functions** — be explicit. If a class manages a resource, declare or delete copy and move operations rather than relying on compiler-generated behaviour:

```cpp
class SerialHandler {
public:
    explicit SerialHandler(const std::string& port);
    ~SerialHandler() = default;

    SerialHandler(const SerialHandler&) = delete;
    SerialHandler& operator=(const SerialHandler&) = delete;

    SerialHandler(SerialHandler&&) = default;
    SerialHandler& operator=(SerialHandler&&) = default;
};
```

---

## Templates

- Template parameter lists use `template <typename T>` with a space after `template`.
- The template declaration and the function signature are always on separate lines.
- Concept constraints are preferred over `enable_if`.
- Use `requires` for constraints that cannot be expressed as a named concept.
- Out-of-class definitions repeat the full `template <...>` prefix without explicit template arguments in the function name.

```cpp
// named concept constraint
template <std::predicate F>
[[nodiscard]] static EError wait(F&& condition);

// requires clause for ad-hoc constraints
template <typename T>
requires std::same_as<T, float> || std::same_as<T, double>
void writeAnalog(T value);

// definition — no <F> after the function name
template <std::predicate F>
auto MyClass::wait(F&& condition) -> EError {
    ...
}
```

---

## Comments

> *"The proper use of comments is to compensate for our failure to express ourselves in code."*
> — Robert C. Martin, Clean Code

If you feel the need to write a comment, the first instinct should be to improve the code instead — rename the function, extract a well-named helper, or restructure the logic until the intent is self-evident. A comment is almost always a sign that the code is not clear enough on its own. Every comment is a failure to write expressive code.

Never write comments that:

- Describe *what* the code does — that is the job of well-named identifiers.
- Explain *who* wrote the code or *when* — that belongs in version control.
- Restate the function signature or variable type.
- Mark work as TODO without a concrete plan — fix it or delete it.

**Commented-out code is unacceptable.** Delete it without hesitation. As Clean Code states, others who see commented-out code will not have the courage to delete it — they will assume it is there for a reason and leave it to rot. Source control remembers everything; if the code needs to come back, it can be recovered from history. Dead code left in comments is noise that erodes trust in the codebase.

---

## Functions

> *"The first rule of functions is that they should be small. The second rule of functions is that they should be smaller than that."*
> — Robert C. Martin, Clean Code

- A function does one thing. If you need "and" to describe what it does, split it.
- Prefer zero or one parameter. Two is acceptable. Three or more is a signal to reconsider the design.
- Never pass a `bool` flag to switch behaviour inside a function — that is two functions in disguise.
- A function either does something (command) or answers something (query), never both. Avoid side effects in query functions.
- Keep functions short enough that the entire body is visible without scrolling.

```cpp
// bad — does two things, bool flag, side effect inside a query
double readAndLog(bool logToFile);

// good — separated responsibilities
double read();
void logReading(double value);
```

---

## Magic Numbers

All numeric literals that carry domain meaning must be named `constexpr` constants. The only numbers acceptable inline are `0` and `1` in loop or index arithmetic where the meaning is unambiguous.

```cpp
// bad
uint8_t pin = pinNumber + 84 + 1;

// good
static constexpr auto s_pinNumberOffset = 84;
static constexpr auto s_zeroIndexCompensate = 1;
uint8_t pin = pinNumber + s_pinNumberOffset + s_zeroIndexCompensate;
```

---

## Resource Management

- Never use raw `new` or `delete`. Acquire resources through constructors and release them through destructors (RAII).
- Use `std::unique_ptr` for exclusive ownership and `std::shared_ptr` only when shared ownership is genuinely required.
- Prefer value semantics and stack allocation over heap allocation.
- Never manage file handles, locks, or hardware resources manually — wrap them in a RAII type.
- When hiding implementation details or breaking compile-time dependencies, use the PIMPL idiom with a forward-declared `struct Impl` and a `std::unique_ptr<Impl> m_pImpl`.

```cpp
class SerialHandler {
public:
    explicit SerialHandler(const std::string& port);
    ~SerialHandler();

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
```

---

## Error Handling

- Use a scoped `EError` enum as a return value for operations that can fail in predictable, recoverable ways.
- Use exceptions only for truly exceptional conditions that the caller cannot reasonably anticipate or recover from.
- Never silently swallow errors. Either propagate them, handle them explicitly, or convert them to a logged diagnostic.
- `[[nodiscard]]` must be applied to every function returning an error code so the compiler enforces that callers check the result.

---

## Includes

Always use C++ style standard library headers (`<cstdint>`, `<cstring>`, `<cmath>`) — never C-style headers (`<stdint.h>`, `<string.h>`, `<math.h>`).

Inside the global module fragment, list standard library headers first, then third-party headers, then project headers. Separate each group with a blank line. Do not include headers that are not directly used in the translation unit.

```cpp
module;

#include <chrono>
#include <cstdint>
#include <thread>

export module timeHelpers;
```

---

## Tests

- One test file per source module, named `test_<module_name>.cpp`.
- Test names use `PascalCase` and describe the expected behaviour: `ReturnNoErrorWhenConditionImmediatelyTrue`.
- Use `TEST_F` with a fixture class when setup or teardown is needed.
- For any test that measures elapsed time, assert both a lower bound (must not return early) and an upper bound (must not run indefinitely).

```cpp
const auto requested = std::chrono::milliseconds(50);
const auto start = std::chrono::steady_clock::now();
functionUnderTest(requested);
const auto elapsed = std::chrono::steady_clock::now() - start;
EXPECT_GE(elapsed, requested);
EXPECT_LT(elapsed, requested + std::chrono::milliseconds(150));
```
