// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include "expression.hpp"
#include "identifier.hpp"

using statement = expression;

struct let_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    const identifier* name {};
    const expression* value {};
};

struct return_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    const expression* value {};
};

struct break_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;
};

struct continue_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;
};

struct expression_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    const expression* expr {};
};

struct block_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expressions statements;
};

struct while_statement final : statement
{
    using expression::expression;
    [[nodiscard]] auto string() const -> std::string override;
    void accept(visitor& visitor) const override;

    expression* condition {};
    block_statement* body {};
};
