module;

#include <type_traits>
#include <string>
#include <string_view>
#include <expected>
#include <source_location>

export module decree;

export namespace Decree {
    #include "decree_impl.inc"
}
