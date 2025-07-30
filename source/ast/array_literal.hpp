#pragma once
#include <vector>

#include "expression.hpp"

struct array_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string final;
    void accept(visitor& visitor) const final;

    std::vector<const expression*> elements;
};
