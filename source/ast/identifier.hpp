// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <utility>
#include <vector>

#include <lexer/location.hpp>

#include "expression.hpp"

struct identifier final : expression
{
    explicit identifier(std::string val, const location& loc)
        : expression {loc}
        , value {std::move(val)}
    {
    }

    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    std::string value;
};

using identifiers = std::vector<const identifier*>;
