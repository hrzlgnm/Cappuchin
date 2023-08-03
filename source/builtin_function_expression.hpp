#pragma once

#include <functional>

#include "callable_expression.hpp"
#include "value_type.hpp"

struct builtin_function_expression : callable_expression
{
    builtin_function_expression(std::string&& name,
                                std::vector<std::string>&& parameters,
                                std::function<object(const array& arguments)>&& body);

    auto call(environment_ptr closure_env,
              environment_ptr caller_env,
              const std::vector<expression_ptr>& arguments) const -> object override;
    auto string() const -> std::string override;

    static const std::vector<builtin_function_expression> builtins;
    std::string name;

    std::function<object(const array& arguments)> body;
};

