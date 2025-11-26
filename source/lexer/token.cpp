// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#include <ostream>

#include "token.hpp"

#include "location.hpp"

auto token::operator==(const token& other) const -> bool
{
    return type == other.type && literal == other.literal && loc == other.loc;
}

auto operator<<(std::ostream& ostream, const token& token) -> std::ostream&
{
    return ostream << "token{" << token.type << ", `" << token.literal << "´ " << token.loc << "}";
}

[[nodiscard]] auto token::with_loc(const location& loc) const -> token
{
    return token {.type = type, .literal = literal, .loc = loc};
}
