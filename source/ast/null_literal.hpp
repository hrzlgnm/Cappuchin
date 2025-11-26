// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <string>

#include "expression.hpp"

struct null_literal final : expression
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;
};
