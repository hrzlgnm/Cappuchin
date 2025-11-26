// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "null_literal.hpp"

#include "visitor.hpp"

auto null_literal::string() const -> std::string
{
    return "null";
}

void null_literal::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
