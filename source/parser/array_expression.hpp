#pragma once
#include <vector>

#include "environment_fwd.hpp"
#include "expression.hpp"

struct array_expression : expression
{
    auto string() const -> std::string override;
    auto eval(environment_ptr env) const -> object override;
    // auto compile(compiler& comp) const -> void override;

    std::vector<expression_ptr> elements;
};
