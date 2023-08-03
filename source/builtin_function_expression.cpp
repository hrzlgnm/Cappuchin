#include <algorithm>
#include <iterator>

#include "builtin_function_expression.hpp"

#include "environment.hpp"
#include "object.hpp"

const std::vector<builtin_function_expression> builtin_function_expression::builtins {
    {"len",
     {"val"},
     [](const array& arguments) -> object
     {
         if (arguments.size() != 1) {
             return make_error("wrong number of arguments to len(): expected=1, got={}", arguments.size());
         }
         return object {std::visit(
             overloaded {
                 [](const string_value& str) -> object { return object(static_cast<integer_value>(str.length())); },
                 [](const array& arr) -> object { return object(static_cast<integer_value>(arr.size())); },
                 [](const auto& other) -> object
                 { return make_error("argument of type {} to len() is not supported", object {other}.type_name()); },
             },
             arguments[0]->value)};
     }},
    {"first",
     {"arr"},
     [](const array& arguments) -> object
     {
         if (arguments.size() != 1) {
             return make_error("wrong number of arguments to first(): expected=1, got={}", arguments.size());
         }
         return object {std::visit(
             overloaded {
                 [](const string_value& str) -> object
                 {
                     if (str.length() > 0) {
                         return object(str.substr(0, 1));
                     }
                     return {};
                 },
                 [](const array& arr) -> object
                 {
                     if (!arr.empty()) {
                         return *arr.front();
                     }
                     return {};
                 },
                 [](const auto& other) -> object
                 { return make_error("argument of type {} to first() is not supported", object {other}.type_name()); },
             },
             arguments[0]->value)};
     }},
    {"last",
     {"arr"},
     [](const array& arguments) -> object
     {
         if (arguments.size() != 1) {
             return make_error("wrong number of arguments to last(): expected=1, got={}", arguments.size());
         }
         return object {std::visit(
             overloaded {
                 [](const string_value& str) -> object
                 {
                     if (str.length() > 1) {
                         return object {str.substr(str.length() - 1, 1)};
                     }
                     return {};
                 },
                 [](const array& arr) -> object
                 {
                     if (!arr.empty()) {
                         return *arr.back();
                     }
                     return {};
                 },
                 [](const auto& other) -> object
                 { return make_error("argument of type {} to last() is not supported", object {other}.type_name()); },
             },
             arguments[0]->value)};
     }},
    {"rest",
     {"arr"},
     [](const array& arguments) -> object
     {
         if (arguments.size() != 1) {
             return make_error("wrong number of arguments to rest(): expected=1, got={}", arguments.size());
         }
         return object {std::visit(
             overloaded {
                 [](const string_value& str) -> object
                 {
                     if (str.size() > 1) {
                         return {str.substr(1)};
                     }
                     return {};
                 },
                 [](const array& arr) -> object
                 {
                     if (arr.size() > 1) {
                         array rest;
                         std::copy(arr.begin() + 1, arr.end(), std::back_inserter(rest));
                         return {rest};
                     }
                     return {};
                 },
                 [](const auto& other) -> object
                 { return make_error("argument of type {} to rest() is not supported", object {other}.type_name()); },
             },
             arguments[0]->value)};
     }},
    {"push",
     {"arr", "val"},
     [](const array& arguments) -> object
     {
         if (arguments.size() != 2) {
             return make_error("wrong number of arguments to push(): expected=2, got={}", arguments.size());
         }
         return std::visit(
             overloaded {
                 [](const array& arr, const auto& obj) -> object
                 {
                     auto copy = arr;
                     copy.push_back(std::make_shared<object>(obj));
                     return {copy};
                 },
                 [](const auto& other1, const auto& other2) -> object
                 {
                     return make_error("argument of type {} and {} to push() are not supported",
                                       object {other1}.type_name(),
                                       object {other2}.type_name());
                 },
             },
             arguments[0]->value,
             arguments[1]->value);
     }},
};

builtin_function_expression::builtin_function_expression(std::string&& name,
                                                         std::vector<std::string>&& parameters,
                                                         std::function<object(const array& arguments)>&& body)
    : callable_expression {std::move(parameters)}
    , name {std::move(name)}
    , body {body}
{
}
auto builtin_function_expression::call(environment_ptr /*closure_env*/,
                                       environment_ptr caller_env,
                                       const std::vector<expression_ptr>& arguments) const -> object
{
    array args;
    std::transform(arguments.cbegin(),
                   arguments.cend(),
                   std::back_inserter(args),
                   [&caller_env](const expression_ptr& expr)
                   { return std::make_shared<object>(expr->eval(caller_env)); });
    return body(args);
};

auto builtin_function_expression::string() const -> std::string
{
    return fmt::format("{}(){}", name, "{...}");
};
