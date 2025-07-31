#pragma once

#include <lexer/token_type.hpp>

#include "expression.hpp"

struct unary_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    token_type op {};
    expression* right {};
};
