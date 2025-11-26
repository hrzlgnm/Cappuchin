// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "decimal_literal.hpp"

#include "util.hpp"
#include "visitor.hpp"

auto decimal_literal::string() const -> std::string
{
    return decimal_to_string(value);
}

void decimal_literal::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
