#pragma once

#include "expression.hpp"

struct program final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string final;
    void accept(struct visitor& visitor) const final;

    expressions statements;
};
