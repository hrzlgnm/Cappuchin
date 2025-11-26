// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "integer_literal.hpp"

#include "visitor.hpp"

auto integer_literal::string() const -> std::string
{
    return std::to_string(value);
}

void integer_literal::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
