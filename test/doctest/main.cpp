// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>

auto main(int argc, char** argv) -> int
{
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    return context.run();
}
