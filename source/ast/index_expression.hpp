#pragma once

#include "expression.hpp"

struct index_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string final;
    void accept(struct visitor& visitor) const final;

    expression* left {};
    expression* index {};
};
