#pragma once
#include <vector>

#include "expression.hpp"

struct array_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    std::vector<const expression*> elements;
};
