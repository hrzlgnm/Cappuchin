// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "identifier.hpp"

#include "visitor.hpp"

auto identifier::string() const -> std::string
{
    return value;
}

void identifier::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
