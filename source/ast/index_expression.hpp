#pragma once

#include "expression.hpp"

struct index_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expression* left {};
    expression* index {};
};
