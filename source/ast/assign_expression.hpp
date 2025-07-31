#pragma once

#include "expression.hpp"
#include "identifier.hpp"

struct assign_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    const identifier* name {};
    const expression* value {};
};
