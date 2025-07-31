#pragma once

#include "expression.hpp"
#include "statements.hpp"

struct if_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expression* condition {};
    block_statement* consequence {};
    block_statement* alternative {};
};
