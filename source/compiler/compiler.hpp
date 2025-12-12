// Copyright 2023-2025 hrzlgnm
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstddef>

#include <ast/program.hpp>
#include <ast/visitor.hpp>
#include <code/code.hpp>
#include <object/object.hpp>

#include "symbol_table.hpp"

using constants = std::vector<const object*>;

struct bytecode final
{
    instructions instrs;
    const constants* consts {};
};

struct emitted_instruction final
{
    opcodes opcode {};
    std::size_t position {};
};

struct compilation_scope final
{
    instructions instrs;
    emitted_instruction last_instr;
    emitted_instruction previous_instr;
};

struct compiler final : visitor
{
    auto compile(const program* program) -> void;
    [[nodiscard]] static auto create() -> compiler;

    [[nodiscard]] static auto create_with_state(constants* constants, symbol_table* symbols) -> compiler
    {
        return compiler {constants, symbols};
    }

    [[nodiscard]] auto add_constant(const object* obj) -> std::size_t;
    [[nodiscard]] auto add_instructions(const instructions& ins) -> std::size_t;

    auto emit(opcodes opcode, const operands& operands = {}) -> std::size_t;

    auto emit(const opcodes opcode, const std::size_t operand) -> std::size_t
    {
        return emit(opcode, std::vector {operand});
    }

    auto emit(const opcodes opcode, const int operand) -> std::size_t
    {
        return emit(opcode, std::vector {static_cast<std::size_t>(operand)});
    }

    [[nodiscard]] auto last_instruction_is(opcodes opcode) const -> bool;
    auto remove_last_pop() -> void;
    auto replace_last_pop_with_return() -> void;
    auto replace_instruction(std::size_t pos, const instructions& instr) -> void;
    auto change_operand(std::size_t pos, std::size_t operand) -> void;
    [[nodiscard]] auto byte_code() const -> bytecode;
    [[nodiscard]] auto current_instrs() const -> const instructions&;
    auto enter_scope(bool inside_loop = false) -> void;
    auto leave_scope() -> instructions;
    auto define_symbol(const std::string& name) -> symbol;
    auto define_function_name(const std::string& name) -> symbol;
    auto load_symbol(const symbol& sym) -> void;
    [[nodiscard]] auto resolve_symbol(const std::string& name) const -> std::optional<symbol>;
    [[nodiscard]] auto free_symbols() const -> std::vector<symbol>;
    [[nodiscard]] auto number_symbol_definitions() const -> int;
    [[nodiscard]] auto consts() const -> constants*;

    [[nodiscard]] auto all_symbols() const -> const symbol_table* { return m_symbols; }

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
    constants* m_consts {};
    symbol_table* m_symbols;
    std::vector<compilation_scope> m_scopes;
    std::size_t m_scope_index {0};
    compiler(constants* consts, symbol_table* symbols);
};
