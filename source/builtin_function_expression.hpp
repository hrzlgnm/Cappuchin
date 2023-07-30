#pragma once
#include <functional>

#include "function_expression.hpp"
#include "node.hpp"
#include "util.hpp"

struct builtin_function_expression : callable_expression
{
    builtin_function_expression(std::string&& name,
                                std::vector<std::string>&& parameters,
                                std::function<object(const std::vector<object>& arguments)>&& body);
    auto call(environment_ptr closure_env,
              environment_ptr caller_env,
              const std::vector<expression_ptr>& arguments) const -> object override;
    auto string() const -> std::string override;
    static std::vector<builtin_function_expression> builtins;
    std::string name;
    std::function<object(const std::vector<object>& arguments)> body;
};