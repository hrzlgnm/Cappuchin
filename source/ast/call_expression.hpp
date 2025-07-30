#pragma once

#include "expression.hpp"

struct call_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string final;
    void accept(visitor& visitor) const final;

    expression* function {};
    expressions arguments;
};
