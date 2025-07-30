#pragma once

#include "expression.hpp"

struct decimal_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string final;
    void accept(visitor& visitor) const final;

    double value {};
};
