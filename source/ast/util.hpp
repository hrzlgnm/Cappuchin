// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

template<typename Expression>
auto join(const std::vector<Expression*>& nodes, const std::string_view sep = {}) -> std::string
{
    auto strs = std::vector<std::string>();
    std::transform(
        nodes.cbegin(), nodes.cend(), std::back_inserter(strs), [](const auto& node) { return node->string(); });
    return fmt::format("{}", fmt::join(strs.cbegin(), strs.cend(), sep));
}

inline auto decimal_to_string(double d) -> std::string
{
    return fmt::format("{}", d);
}
