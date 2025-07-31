#pragma once

#include "expression.hpp"

struct call_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expression* function {};
    expressions arguments;
};
