module;

#include <type_traits>
#include <string>
#include <expected>

export module errors;

export namespace Errors {
    #include "decree_impl.inc"
}
