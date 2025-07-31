#pragma once

#include <ast/program.hpp>
#include <ast/visitor.hpp>
#include <compiler/symbol_table.hpp>
#include <eval/environment.hpp>

struct analyzer final : visitor
{
    explicit analyzer(symbol_table* symbols);
    void analyze(const program* prgrm) noexcept(false);

    void visit(const array_literal& expr) override;
    void visit(const assign_expression& expr) override;
    void visit(const binary_expression& expr) override;
    void visit(const block_statement& expr) override;
    void visit(const break_statement& expr) override;
    void visit(const call_expression& expr) override;
    void visit(const continue_statement& expr) override;
    void visit(const expression_statement& expr) override;
    void visit(const function_literal& expr) override;
    void visit(const hash_literal& expr) override;
    void visit(const identifier& expr) override;
    void visit(const if_expression& expr) override;
    void visit(const index_expression& expr) override;
    void visit(const let_statement& expr) override;
    void visit(const program& expr) override;
    void visit(const return_statement& expr) override;
    void visit(const unary_expression& expr) override;
    void visit(const while_statement& expr) override;

    void visit(const boolean_literal& /* expr */) override {}

    void visit(const decimal_literal& /* expr */) override {}

    void visit(const integer_literal& /* expr */) override {}

    void visit(const null_literal& /* expr */) override {}

    void visit(const string_literal& /* expr */) override {}

  private:
    symbol_table* m_symbols;
};

void analyze_program(const program* program,
                     symbol_table* existing_symbols,
                     const environment* existing_env) noexcept(false);
