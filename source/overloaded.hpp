// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

// helper type for std::visit
template<typename... T>
struct overloaded : T...
{
    using T::operator()...;
};
template<class... T>
overloaded(T...) -> overloaded<T...>;
