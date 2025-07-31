#pragma once

#include <string>
#include <utility>

#include <lexer/location.hpp>

#include "expression.hpp"

struct string_literal final : expression
{
    string_literal(std::string val, const location& loc)
        : expression {loc}
        , value {std::move(val)}
    {
    }

    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    std::string value;
};
