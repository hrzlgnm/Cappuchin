// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "boolean_literal.hpp"

#include "visitor.hpp"

auto boolean_literal::string() const -> std::string
{
    return std::string {value ? "true" : "false"};
}

void boolean_literal::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
