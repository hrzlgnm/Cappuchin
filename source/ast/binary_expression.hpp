// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once
#include <string>

#include <lexer/token_type.hpp>

#include "expression.hpp"

struct binary_expression final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expression* left {};
    token_type op {};
    expression* right {};
};
