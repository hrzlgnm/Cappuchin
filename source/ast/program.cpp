// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <string>

#include "program.hpp"

#include <fmt/format.h>

#include "util.hpp"
#include "visitor.hpp"

auto program::string() const -> std::string
{
    return fmt::format("{}", join(statements));
}

void program::accept(visitor& visitor) const
{
    visitor.visit(*this);
}
