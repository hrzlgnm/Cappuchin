// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <utility>
#include <vector>

#include "expression.hpp"

struct hash_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    std::vector<std::pair<expression*, expression*>> pairs;
};
