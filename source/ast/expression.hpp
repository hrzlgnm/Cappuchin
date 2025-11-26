// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <string>
#include <vector>

#include <lexer/location.hpp>

struct visitor;

struct expression
{
    explicit expression(const location& loc)
        : l {loc}
    {
    }

    virtual ~expression() = default;
    expression(const expression&) = delete;
    expression(expression&&) = delete;
    auto operator=(const expression&) -> expression& = delete;
    auto operator=(expression&&) -> expression& = delete;

    [[nodiscard]] virtual auto string() const -> std::string = 0;
    virtual void accept(visitor& visitor) const = 0;

    [[nodiscard]] auto loc() const -> const location& { return l; }

    location l;
};

using expressions = std::vector<const expression*>;
