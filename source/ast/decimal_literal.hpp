#pragma once

#include "expression.hpp"

struct decimal_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    double value {};
};
