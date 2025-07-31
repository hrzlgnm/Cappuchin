#pragma once

#include <string>

#include "expression.hpp"

struct null_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;
};
