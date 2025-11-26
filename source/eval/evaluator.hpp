// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <ast/expression.hpp>
#include <ast/program.hpp>
#include <ast/visitor.hpp>
#include <object/object.hpp>

#include "environment.hpp"

struct evaluator final : visitor
{
    explicit evaluator(environment* existing_env = nullptr);
    auto evaluate(const program* prgrm) -> const object*;

  protected:
    void visit(const array_literal& expr) override;
    void visit(const assign_expression& expr) override;
    void visit(const binary_expression& expr) override;
    void visit(const block_statement& expr) override;
    void visit(const boolean_literal& expr) override;
    void visit(const break_statement& expr) override;
    void visit(const call_expression& expr) override;
    void visit(const continue_statement& expr) override;
    void visit(const decimal_literal& expr) override;
    void visit(const expression_statement& expr) override;
    void visit(const function_literal& expr) override;
    void visit(const hash_literal& expr) override;
    void visit(const identifier& expr) override;
    void visit(const if_expression& expr) override;
    void visit(const index_expression& expr) override;
    void visit(const integer_literal& expr) override;
    void visit(const let_statement& expr) override;
    void visit(const null_literal& expr) override;
    void visit(const program& expr) override;
    void visit(const return_statement& expr) override;
    void visit(const string_literal& expr) override;
    void visit(const unary_expression& expr) override;
    void visit(const while_statement& expr) override;

  private:
    void apply_function(const object* function_or_builtin, array_object::value_type&& args);
    auto evaluate_expressions(const expressions& exprs) -> array_object::value_type;
    environment* m_env {};
    const object* m_result {};
};
