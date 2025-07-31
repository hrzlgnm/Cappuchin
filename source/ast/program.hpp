#pragma once

#include "expression.hpp"

struct program final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expressions statements;
};
