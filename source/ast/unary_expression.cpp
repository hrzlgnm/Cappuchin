// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "unary_expression.hpp"

#include <fmt/format.h>

#include "visitor.hpp"

auto unary_expression::string() const -> std::string
{
    return fmt::format("({}{})", op, right->string());
}

void unary_expression::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
