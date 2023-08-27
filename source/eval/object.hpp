#pragma once

#include <algorithm>
#include <any>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <code/code.hpp>
#include <fmt/core.h>

#include "compiler/symbol_table.hpp"
#include "environment_fwd.hpp"

// helper type for std::visit
template<typename... T>
struct overloaded : T...
{
    using T::operator()...;
};
template<class... T>
overloaded(T...) -> overloaded<T...>;

using null_type = std::monostate;

struct error
{
    std::string message;
};

auto operator==(const error& lhs, const error& rhs) -> bool;

using integer_type = std::int64_t;
using string_type = std::string;
struct object;
using array = std::vector<object>;
using hash_key_type = std::variant<integer_type, char, string_type, bool>;
using hash = std::unordered_map<hash_key_type, std::any>;

struct callable_expression;
using bound_function = std::pair<const callable_expression*, environment_ptr>;
struct builtin_function_expression;

struct compiled_function
{
    instructions instrs;
    size_t num_locals {};
    size_t num_arguments {};
};

struct closure
{
    compiled_function fn;
    array free;
};

using value_type = std::variant<null_type,
                                bool,
                                integer_type,
                                string_type,
                                error,
                                array,
                                hash,
                                bound_function,
                                compiled_function,
                                closure,
                                const builtin_function_expression*>;

namespace std
{
auto to_string(const value_type& value) -> std::string;
}  // namespace std

struct object
{
    template<typename T>
    [[nodiscard]] inline constexpr auto is() const -> bool
    {
        return std::holds_alternative<T>(value);
    }

    template<typename T>
    [[nodiscard]] auto as() const -> const T&
    {
        if (!is<T>()) {
            throw std::runtime_error(
                fmt::format("cannot convert {} to {}", std::to_string(value), object {T {}}.type_name()));
        }
        return std::get<T>(value);
    }

    [[nodiscard]] inline constexpr auto is_nil() const -> bool { return is<null_type>(); }

    [[nodiscard]] [[nodiscard]] inline constexpr auto is_hashable() const -> bool
    {
        return is<integer_type>() || is<string_type>() || is<bool>();
    }

    [[nodiscard]] inline auto is_truthy() const -> bool { return !is_nil() && (!is<bool>() || as<bool>()); }

    [[nodiscard]] inline auto hash_key() const -> hash_key_type
    {
        return std::visit(overloaded {[](const integer_type integer) -> hash_key_type { return {integer}; },
                                      [](const string_type& str) -> hash_key_type { return {str}; },
                                      [](const bool val) -> hash_key_type { return {val}; },
                                      [](const auto& other) -> hash_key_type {
                                          throw std::invalid_argument(object {other}.type_name() + " is not hashable");
                                      }},
                          value);
    }

    [[nodiscard]] auto type_name() const -> std::string;

    value_type value {};
    bool is_return_value {};
};

static const null_type null_value {};
static const object null {null_value};
auto unwrap(const std::any& obj) -> object;

template<typename... T>
auto make_error(fmt::format_string<T...> fmt, T&&... args) -> object
{
    return {error {.message = fmt::format(fmt, std::forward<T>(args)...)}};
}

inline constexpr auto operator==(const object& lhs, const object& rhs) -> bool
{
    return std::visit(
        overloaded {[](const null_type, const null_type) { return true; },
                    [](const bool val1, const bool val2) { return val1 == val2; },
                    [](const integer_type val1, const integer_type val2) { return val1 == val2; },
                    [](const string_type& val1, const string_type& val2) { return val1 == val2; },
                    [](const bound_function& /*val1*/, const bound_function& /*val2*/) { return false; },
                    [](const error& err1, const error& err2) { return err1.message == err2.message; },
                    [](const array& arr1, const array& arr2)
                    { return arr1.size() == arr2.size() && std::equal(arr1.cbegin(), arr1.cend(), arr2.begin()); },
                    [](const auto&, const auto&) { return false; }},
        lhs.value,
        rhs.value);
}
