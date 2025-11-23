#pragma once

#include "expression.hpp"

struct boolean_literal final : expression
{
    explicit boolean_literal(const bool val, const location& loc)
        : expression {loc}
        , value {val}
    {
    }

    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    bool value {};
};
