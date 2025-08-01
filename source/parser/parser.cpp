#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "parser.hpp"

#include <ast/array_literal.hpp>
#include <ast/assign_expression.hpp>
#include <ast/binary_expression.hpp>
#include <ast/boolean_literal.hpp>
#include <ast/call_expression.hpp>
#include <ast/decimal_literal.hpp>
#include <ast/expression.hpp>
#include <ast/function_literal.hpp>
#include <ast/hash_literal.hpp>
#include <ast/identifier.hpp>
#include <ast/if_expression.hpp>
#include <ast/index_expression.hpp>
#include <ast/integer_literal.hpp>
#include <ast/null_literal.hpp>
#include <ast/program.hpp>
#include <ast/statements.hpp>
#include <ast/string_literal.hpp>
#include <ast/unary_expression.hpp>
#include <ast/util.hpp>
#include <doctest/doctest.h>
#include <fmt/ranges.h>
#include <gc.hpp>
#include <lexer/lexer.hpp>
#include <lexer/token.hpp>
#include <lexer/token_type.hpp>
#include <overloaded.hpp>

#include "lexer/location.hpp"

namespace
{
enum precedence : std::uint8_t
{
    lowest,
    equals,
    lessgreater,
    bitwise_or,
    bitwise_xor,
    bitwise_and,
    bitwise_shift,
    logical_or,
    logical_and,
    sum,
    product,
    prefix,
    call,
    idx,
};

auto precedence_of_token(const token_type type) -> std::uint8_t
{
    switch (type) {
        case token_type::equals:
        case token_type::not_equals:
        case token_type::less_equal:
        case token_type::greater_equal:
            return equals;
        case token_type::less_than:
        case token_type::greater_than:
            return lessgreater;
        case token_type::plus:
        case token_type::minus:
            return sum;
        case token_type::logical_and:
            return logical_and;
        case token_type::logical_or:
            return logical_or;
        case token_type::pipe:
            return bitwise_or;
        case token_type::ampersand:
            return bitwise_and;
        case token_type::caret:
            return bitwise_xor;
        case token_type::shift_left:
        case token_type::shift_right:
            return bitwise_shift;
        case token_type::double_slash:
        case token_type::slash:
        case token_type::asterisk:
        case token_type::percent:
            return product;
        case token_type::lparen:
            return call;
        case token_type::lbracket:
            return idx;
        default:
            return lowest;
    }
}
}  // namespace

parser::parser(const lexer& lxr)
    : m_lxr {lxr}
{
    next_token();
    next_token();
    using enum token_type;
    register_unary(ident, [this] { return parse_identifier(); });
    register_unary(integer, [this] { return parse_integer_literal(); });
    register_unary(decimal, [this] { return parse_decimal_literal(); });
    register_unary(exclamation, [this] { return parse_unary_expression(); });
    register_unary(minus, [this] { return parse_unary_expression(); });
    register_unary(tru, [this] { return parse_boolean(); });
    register_unary(fals, [this] { return parse_boolean(); });
    register_unary(lparen, [this] { return parse_grouped_expression(); });
    register_unary(eef, [this] { return parse_if_expression(); });
    register_unary(function, [this] { return parse_function_expression(); });
    register_unary(string, [this] { return parse_string_literal(); });
    register_unary(lbracket, [this] { return parse_array_expression(); });
    register_unary(lsquirly, [this] { return parse_hash_literal(); });
    register_unary(null, [this] { return parse_null_literal(); });
    register_binary(plus, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(minus, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(slash, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(asterisk, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(equals, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(not_equals, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(less_than, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(greater_than, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(lparen, [this](expression* left) { return parse_call_expression(left); });
    register_binary(lbracket, [this](expression* left) { return parse_index_expression(left); });
    register_binary(double_slash, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(percent, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(ampersand, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(pipe, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(caret, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(shift_left, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(shift_right, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(logical_and, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(logical_or, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(greater_equal, [this](expression* left) { return parse_binary_expression(left); });
    register_binary(less_equal, [this](expression* left) { return parse_binary_expression(left); });
}

auto parser::parse_program() -> program*
{
    auto* prog = allocate<program>(m_current_token.loc);
    while (m_current_token.type != token_type::eof) {
        if (const auto* stmt = parse_statement(); stmt != nullptr) {
            prog->statements.push_back(stmt);
        }
        next_token();
    }
    return prog;
}

auto parser::errors() const -> const std::vector<std::string>&
{
    return m_errors;
}

auto parser::next_token() -> void
{
    m_current_token = m_peek_token;
    m_peek_token = m_lxr.next_token();
}

auto parser::parse_statement() -> statement*
{
    using enum token_type;
    if (current_token_is(let)) {
        return parse_let_statement();
    }
    if (current_token_is(ident) && peek_token_is(assign)) {
        return parse_assign_statement();
    }
    if (current_token_is(ret)) {
        return parse_return_statement();
    }
    if (current_token_is(hwile)) {
        return parse_while_statement();
    }
    if (current_token_is(brake)) {
        return parse_break_statement();
    }
    if (current_token_is(cont)) {
        return parse_continue_statement();
    }
    return parse_expression_statement();
}

auto parser::parse_let_statement() -> statement*
{
    auto* stmt = allocate<let_statement>(m_current_token.loc);
    stmt->l = m_current_token.loc;
    using enum token_type;
    if (!get(ident)) {
        return {};
    }
    stmt->name = parse_identifier();

    if (!get(assign)) {
        return {};
    }

    next_token();
    auto* expr = parse_expression(lowest);
    if (auto* func_expr = dynamic_cast<function_literal*>(expr); func_expr != nullptr) {
        func_expr->name = stmt->name->value;
    }
    stmt->value = expr;

    if (peek_token_is(semicolon)) {
        next_token();
    }
    return stmt;
}

auto parser::parse_assign_statement() -> statement*
{
    auto* stmt = allocate<assign_expression>(m_current_token.loc);
    using enum token_type;

    stmt->name = parse_identifier();

    if (!get(assign)) {
        return {};
    }

    next_token();
    stmt->value = parse_expression(lowest);

    if (peek_token_is(semicolon)) {
        next_token();
    }
    return stmt;
}

auto parser::parse_return_statement() -> statement*
{
    using enum token_type;
    auto* stmt = allocate<return_statement>(m_current_token.loc);

    next_token();
    stmt->value = parse_expression(lowest);

    if (peek_token_is(semicolon)) {
        next_token();
    }
    return stmt;
}

auto parser::parse_expression_statement() -> statement*
{
    auto* expr_stmt = allocate<expression_statement>(m_current_token.loc);
    expr_stmt->expr = parse_expression(lowest);
    if (peek_token_is(token_type::semicolon)) {
        next_token();
    }
    return expr_stmt;
}

auto parser::parse_expression(const int precedence) -> expression*
{
    const auto unary = m_unary_parsers[m_current_token.type];
    if (!unary) {
        no_unary_expression_error(m_current_token.type);
        return {};
    }
    auto* left_expr = unary();
    while (!peek_token_is(token_type::semicolon) && precedence < peek_precedence()) {
        auto binary = m_binary_parsers[m_peek_token.type];
        if (!binary) {
            return left_expr;
        }
        next_token();

        left_expr = binary(left_expr);
    }
    return left_expr;
}

auto parser::parse_identifier() const -> identifier*
{
    return allocate<identifier>(std::string(m_current_token.literal), m_current_token.loc);
}

auto parser::parse_integer_literal() -> expression*
{
    auto* lit = allocate<integer_literal>(m_current_token.loc);
    try {
        lit->value = std::stoll(std::string {m_current_token.literal});
    } catch (const std::out_of_range&) {
        new_error("{}: could not parse {} as integer", m_current_token.loc, m_current_token.literal);
        return {};
    }
    return lit;
}

auto parser::parse_decimal_literal() -> expression*
{
    auto* lit = allocate<decimal_literal>(m_current_token.loc);
    try {
        lit->value = std::stod(std::string {m_current_token.literal});
    } catch (const std::out_of_range&) {
        new_error("{}: could not parse {} as decimal", m_current_token.loc, m_current_token.literal);
        return {};
    }
    return lit;
}

auto parser::parse_unary_expression() -> expression*
{
    auto* unary = allocate<unary_expression>(m_current_token.loc);
    unary->op = m_current_token.type;

    next_token();
    unary->right = parse_expression(prefix);
    return unary;
}

auto parser::parse_boolean() -> expression*
{
    return allocate<boolean_literal>(current_token_is(token_type::tru), m_current_token.loc);
}

auto parser::parse_grouped_expression() -> expression*
{
    next_token();
    auto* exp = parse_expression(lowest);
    if (!get(token_type::rparen)) {
        return {};
    }
    return exp;
}

auto parser::parse_if_expression() -> expression*
{
    using enum token_type;
    auto* expr = allocate<if_expression>(m_current_token.loc);
    if (!get(lparen)) {
        return {};
    }
    next_token();
    expr->condition = parse_expression(lowest);

    if (!get(rparen)) {
        return {};
    }

    if (!get(lsquirly)) {
        return {};
    }

    expr->consequence = parse_block_statement();

    if (peek_token_is(elze)) {
        next_token();
        if (!get(lsquirly)) {
            return {};
        }
        expr->alternative = parse_block_statement();
    }

    return expr;
}

auto parser::parse_while_statement() -> expression*
{
    using enum token_type;
    auto* expr = allocate<while_statement>(m_current_token.loc);
    if (!get(lparen)) {
        return {};
    }
    next_token();
    expr->condition = parse_expression(lowest);

    if (!get(rparen)) {
        return {};
    }

    if (!get(lsquirly)) {
        return {};
    }

    expr->body = parse_block_statement();

    return expr;
}

auto parser::parse_function_expression() -> expression*
{
    using enum token_type;
    const auto loc = m_current_token.loc;
    if (!get(lparen)) {
        return {};
    }
    auto parameters = parse_function_parameters();
    if (!get(lsquirly)) {
        return {};
    }
    auto* body = parse_block_statement();
    return allocate<function_literal>(std::move(parameters), body, loc);
}

auto parser::parse_function_parameters() -> identifiers
{
    using enum token_type;
    identifiers parameters;
    if (peek_token_is(rparen)) {
        next_token();
        return parameters;
    }
    next_token();
    const auto* param = parse_identifier();
    parameters.push_back(param);
    while (peek_token_is(comma)) {
        next_token();
        next_token();
        param = parse_identifier();
        parameters.push_back(param);
    }
    if (!get(rparen)) {
        return {};
    }
    return parameters;
}

auto parser::parse_block_statement() -> block_statement*
{
    using enum token_type;
    auto* block = allocate<block_statement>(m_current_token.loc);
    next_token();
    while (!current_token_is(rsquirly) && !current_token_is(eof)) {
        if (const auto* stmt = parse_statement(); stmt != nullptr) {
            block->statements.push_back(stmt);
        }
        next_token();
    }

    return block;
}

auto parser::parse_break_statement() -> statement*
{
    using enum token_type;
    auto* b = allocate<break_statement>(m_current_token.loc);
    if (peek_token_is(semicolon)) {
        next_token();
    }
    return b;
}

auto parser::parse_continue_statement() -> statement*
{
    using enum token_type;
    auto* b = allocate<continue_statement>(m_current_token.loc);
    if (peek_token_is(semicolon)) {
        next_token();
    }
    return b;
}

auto parser::parse_call_expression(expression* function) -> expression*
{
    auto* call = allocate<call_expression>(m_current_token.loc);
    call->function = function;
    call->arguments = parse_expressions(token_type::rparen);
    return call;
}

auto parser::parse_binary_expression(expression* left) -> expression*
{
    auto* bin_expr = allocate<binary_expression>(m_current_token.loc);
    bin_expr->op = m_current_token.type;
    bin_expr->left = left;

    const auto precedence = current_precedence();
    next_token();
    bin_expr->right = parse_expression(precedence);

    return bin_expr;
}

auto parser::parse_string_literal() const -> expression*
{
    return allocate<string_literal>(std::string {m_current_token.literal}, m_current_token.loc);
}

auto parser::parse_expressions(const token_type end) -> expressions
{
    using enum token_type;
    expressions list;
    if (peek_token_is(end)) {
        next_token();
        return list;
    }
    next_token();
    list.push_back(parse_expression(lowest));

    while (peek_token_is(comma)) {
        next_token();
        next_token();
        list.push_back(parse_expression(lowest));
    }

    if (!get(end)) {
        return {};
    }

    return list;
}

auto parser::parse_array_expression() -> expression*
{
    auto* array_expr = allocate<array_literal>(m_current_token.loc);
    array_expr->elements = parse_expressions(token_type::rbracket);
    return array_expr;
}

auto parser::parse_index_expression(expression* left) -> expression*
{
    auto* index_expr = allocate<index_expression>(m_current_token.loc);
    index_expr->left = left;
    next_token();
    index_expr->index = parse_expression(lowest);

    if (!get(token_type::rbracket)) {
        return {};
    }

    return index_expr;
}

auto parser::parse_hash_literal() -> expression*
{
    auto* hash = allocate<hash_literal>(m_current_token.loc);
    using enum token_type;
    while (!peek_token_is(rsquirly)) {
        next_token();
        auto* key = parse_expression(lowest);
        if (!get(colon)) {
            return {};
        }
        next_token();
        auto* value = parse_expression(lowest);
        hash->pairs.emplace_back(key, value);
        if (!peek_token_is(comma)) {
            break;
        }
        next_token();
    }
    if (!get(rsquirly)) {
        return {};
    }
    return hash;
}

auto parser::parse_null_literal() -> expression*
{
    return allocate<null_literal>(m_current_token.loc);
}

auto parser::get(const token_type type) -> bool
{
    if (m_peek_token.type == type) {
        next_token();
        return true;
    }
    peek_error(type);
    return false;
}

auto parser::peek_error(token_type type) -> void
{
    new_error("{}: expected next token to be {}, got {} instead", m_current_token.loc, type, m_peek_token.literal);
}

auto parser::register_binary(const token_type type, binary_parser binary) -> void
{
    m_binary_parsers[type] = std::move(binary);
}

auto parser::register_unary(const token_type type, unary_parser unary) -> void
{
    m_unary_parsers[type] = std::move(unary);
}

auto parser::current_token_is(const token_type type) const -> bool
{
    return m_current_token.type == type;
}

auto parser::peek_token_is(const token_type type) const -> bool
{
    return m_peek_token.type == type;
}

auto parser::no_unary_expression_error(token_type /*type*/) -> void
{
    new_error("{}: no prefix parse function for {} found", m_current_token.loc, m_current_token.literal);
}

auto parser::peek_precedence() const -> int
{
    return precedence_of_token(m_peek_token.type);
}

auto parser::current_precedence() const -> int
{
    return precedence_of_token(m_current_token.type);
}

namespace
{
// NOLINTBEGIN(*)
using expected_value_type = std::variant<int64_t, std::string, bool, double>;

auto check_no_parse_errors(const parser& prsr) -> bool
{
    INFO("expected no errors, got:", fmt::format("{}", fmt::join(prsr.errors(), ", ")));
    CHECK(prsr.errors().empty());
    return prsr.errors().empty();
}

using parsed_program = std::pair<program*, parser>;

auto check_program(std::string_view input) -> parsed_program
{
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    INFO("while parsing: `", input, "`");
    CHECK(check_no_parse_errors(prsr));
    return {std::move(prgrm), std::move(prsr)};
}

auto require_boolean_literal(const expression* expr, bool value) -> void
{
    auto* bool_expr = dynamic_cast<const boolean_literal*>(expr);
    INFO("expected boolean, got:", expr->string());
    REQUIRE(bool_expr);
    REQUIRE_EQ(bool_expr->value, value);
}

auto require_identifier(const expression* expr, const std::string& value) -> void
{
    auto* ident = dynamic_cast<const identifier*>(expr);
    INFO("expected identifier, got:", expr->string());
    REQUIRE(ident);
    REQUIRE_EQ(ident->value, value);
}

auto require_string_literal(const expression* expr, const std::string& value) -> void
{
    auto* string_lit = dynamic_cast<const string_literal*>(expr);
    INFO("expected string_literal, got:", expr->string());
    REQUIRE(string_lit);
    REQUIRE_EQ(string_lit->value, value);
}

auto require_integer_literal(const expression* expr, int64_t value) -> void
{
    auto* integer_lit = dynamic_cast<const integer_literal*>(expr);
    INFO("expected integer_literal, got:", expr->string());
    REQUIRE(integer_lit);

    REQUIRE_EQ(integer_lit->value, value);
    REQUIRE_EQ(integer_lit->string(), std::to_string(value));
}

auto require_decimal_literal(const expression* expr, double value) -> void
{
    auto* decimal_lit = dynamic_cast<const decimal_literal*>(expr);
    INFO("expected integer_literal, got:", expr->string());
    REQUIRE(decimal_lit);

    REQUIRE_EQ(decimal_lit->value, value);
    REQUIRE_EQ(decimal_lit->string(), decimal_to_string(value));
}

auto require_literal_expression(const expression* expr, const expected_value_type& expected) -> void
{
    std::visit(
        overloaded {
            [&](int64_t val) { require_integer_literal(expr, val); },
            [&](const std::string& val) { require_identifier(expr, val); },
            [&](bool val) { require_boolean_literal(expr, val); },
            [&](double val) { require_decimal_literal(expr, val); },
        },
        expected);
}

auto require_binary_expression(const expression* expr,
                               const expected_value_type& left,
                               const token_type oprtr,
                               const expected_value_type& right) -> void
{
    auto* binary = dynamic_cast<const binary_expression*>(expr);
    INFO("expected binary expression, got: ", expr->string());
    REQUIRE(binary);
    require_literal_expression(binary->left, left);
    REQUIRE_EQ(binary->op, oprtr);
    require_literal_expression(binary->right, right);
}

auto require_expression_statement(const program* prgrm) -> const expression_statement*
{
    INFO("expected one statement, got: ", prgrm->statements.size());
    REQUIRE_EQ(prgrm->statements.size(), 1);
    auto* stmt = prgrm->statements[0];
    auto* expr_stmt = dynamic_cast<const expression_statement*>(stmt);
    INFO("expected expression statement, got: ", stmt->string());
    REQUIRE(expr_stmt);
    INFO("expected an expression statement with an expression");
    WARN(expr_stmt->expr);
    return expr_stmt;
}

template<typename E>
auto require_expression(const program* prgrm) -> const E*
{
    auto* expr_stmt = require_expression_statement(prgrm);
    auto* expr = dynamic_cast<const E*>(expr_stmt->expr);
    INFO("expected expression");
    REQUIRE(expr);
    return expr;
}

template<typename E>
auto require_statement(const program* prgrm) -> const E*
{
    INFO("expected one statement, got: ", prgrm->statements.size());
    REQUIRE_EQ(prgrm->statements.size(), 1);
    auto* stmt = prgrm->statements[0];
    auto* actual_stmt = dynamic_cast<const E*>(stmt);
    INFO("expected statement, got: ", stmt->string());
    REQUIRE(actual_stmt);
    return actual_stmt;
}

auto require_let_statement(const statement* stmt, const std::string& expected_identifier) -> const let_statement*
{
    auto* let_stmt = dynamic_cast<const let_statement*>(stmt);
    INFO("expected let statement, got: ", stmt->string());
    REQUIRE(let_stmt);
    REQUIRE_EQ(let_stmt->name->value, expected_identifier);
    return let_stmt;
}

auto require_assign_expression(const statement* stmt, const std::string& expected_identifier)
    -> const assign_expression*
{
    auto* assign_expr = dynamic_cast<const assign_expression*>(stmt);
    INFO("expected assign expression, got: ", stmt->string());
    REQUIRE(assign_expr);
    REQUIRE_EQ(assign_expr->name->value, expected_identifier);
    return assign_expr;
}

TEST_SUITE_BEGIN("parsing");

TEST_CASE("letStatement")
{
    using enum token_type;
    auto [program, prsr] = check_program(
        R"(
let x = 5;
let y = 10;
let foobar = 838383;
        )");
    REQUIRE_EQ(program->statements.size(), 3);
    const auto expected_identifiers = std::vector<std::string> {
        "x",
        "y",
        "foobar",
    };
    for (size_t i = 0; i < 3; ++i) {
        require_let_statement(program->statements[i], expected_identifiers[i]);
    }
}

TEST_CASE("assignExpression")
{
    using enum token_type;
    auto [program, prsr] = check_program(
        R"(
x = 5;
y = 10;
foobar = 838383;
        )");
    REQUIRE_EQ(program->statements.size(), 3);
    const auto expected_identifiers = std::vector<std::string> {"x", "y", "foobar", "x"};
    for (size_t i = 0; i < 3; ++i) {
        require_assign_expression(program->statements[i], expected_identifiers[i]);
    }
}

TEST_CASE("letStatements")
{
    struct lt
    {
        std::string_view input;
        std::string expected_identifier;
        expected_value_type expected_value;
    };

    std::array tests {
        lt {"let x = 5;", "x", 5},
        lt {"let y = true;", "y", true},
        lt {"let foobar = y;", "foobar", "y"},
    };

    for (const auto& [input, expected_identifier, expected_value] : tests) {
        auto [prgrm, _] = check_program(input);
        auto* let_stmt = require_let_statement(prgrm->statements[0], expected_identifier);
        require_literal_expression(let_stmt->value, expected_value);
    }
}

TEST_CASE("assignmentExpressions")
{
    struct lt
    {
        std::string_view input;
        std::string expected_identifier;
        expected_value_type expected_value;
    };

    std::array tests {
        lt {"x = 5;", "x", 5},
        lt {"y = true;", "y", true},
        lt {"foobar = y;", "foobar", "y"},
    };

    for (const auto& [input, expected_identifier, expected_value] : tests) {
        auto [prgrm, _] = check_program(input);
        auto* assign_expr = require_assign_expression(prgrm->statements[0], expected_identifier);
        require_literal_expression(assign_expr->value, expected_value);
    }
}

TEST_CASE("parseError")
{
    using enum token_type;
    auto prsr = parser {lexer {
        R"(
let x = 5;
let y = 10;
let 838383;
        )"}};
    prsr.parse_program();
    const auto errors = prsr.errors();
    CHECK_FALSE(errors.empty());
}

TEST_CASE("returnStatement")
{
    using enum token_type;
    auto [prgrm, _] = check_program(
        R"(
return 5;
return 10;
return 993322;
        )");
    REQUIRE_EQ(prgrm->statements.size(), 3);
    std::array expected_return_values {5, 10, 993322};
    for (size_t i = 0; i < 3; ++i) {
        auto* stmt = prgrm->statements[i];
        auto* ret_stmt = dynamic_cast<const return_statement*>(stmt);
        REQUIRE(ret_stmt);
        require_literal_expression(ret_stmt->value, expected_return_values[i]);
    }
}

TEST_CASE("string")
{
    using enum token_type;
    const auto name = allocate<identifier>("myVar", location {"<stdin>", 1, 1});
    const auto value = allocate<identifier>("anotherVar", location {"<stdin>", 1, 9});

    program prgrm {location {"<stdin>", 1, 1}};

    const auto let_stmt = allocate<let_statement>(location {});

    let_stmt->name = name;
    let_stmt->value = value;
    prgrm.statements.push_back(let_stmt);

    REQUIRE_EQ(prgrm.string(), "let myVar = anotherVar;");
}

TEST_CASE("identifierExpression")
{
    const auto* input = "foobar;";
    auto [prgrm, _] = check_program(input);
    auto* expr_stmt = require_expression_statement(prgrm);

    require_literal_expression(expr_stmt->expr, "foobar");
}

TEST_CASE("integerExpression")
{
    auto [prgrm, _] = check_program("5;");
    auto* expr_stmt = require_expression_statement(prgrm);

    require_literal_expression(expr_stmt->expr, 5);
}

TEST_CASE("decimalExpression")
{
    auto [prgrm, _] = check_program("5.5;");
    auto* expr_stmt = require_expression_statement(prgrm);

    require_literal_expression(expr_stmt->expr, 5.5);
}

TEST_CASE("unaryExpressions")
{
    using enum token_type;

    struct ut
    {
        std::string_view input;
        token_type op;
        int64_t integer_value;
    };

    std::array tests {
        ut {"!5;", exclamation, 5},
        ut {"-15;", minus, 15},
    };

    for (const auto& [input, op, val] : tests) {
        auto [prgrm, _] = check_program(input);
        auto unary = require_expression<unary_expression>(prgrm);
        REQUIRE(unary);
        REQUIRE_EQ(op, unary->op);

        require_literal_expression(unary->right, val);
    }
}

TEST_CASE("binaryExpressions")
{
    using enum token_type;

    struct bt
    {
        std::string_view input;
        int64_t left_value;
        token_type op;
        int64_t right_value;
    };

    std::array tests {
        bt {"5 + 5;", 5, plus, 5},           bt {"5 - 5;", 5, minus, 5},         bt {"5 * 5;", 5, asterisk, 5},
        bt {"5 / 5;", 5, slash, 5},          bt {"5 > 5;", 5, greater_than, 5},  bt {"5 < 5;", 5, less_than, 5},
        bt {"5 >= 5;", 5, greater_equal, 5}, bt {"5 <= 5;", 5, less_equal, 5},   bt {"5 == 5;", 5, equals, 5},
        bt {"5 != 5;", 5, not_equals, 5},    bt {"5 // 5;", 5, double_slash, 5}, bt {"5 % 5;", 5, percent, 5},
        bt {"5 & 5;", 5, ampersand, 5},      bt {"5 | 5;", 5, pipe, 5},          bt {"5 ^ 5;", 5, caret, 5},
        bt {"5 << 5;", 5, shift_left, 5},    bt {"5 >> 5;", 5, shift_right, 5},  bt {"5 && 5;", 5, logical_and, 5},
        bt {"5 || 5;", 5, logical_or, 5},
    };

    for (const auto& [input, left, op, right] : tests) {
        auto [prgrm, _] = check_program(input);
        auto* expr_stmt = require_expression_statement(prgrm);

        require_binary_expression(expr_stmt->expr, left, op, right);
    }
}

TEST_CASE("operatorPrecedence")
{
    struct op
    {
        std::string_view input;
        std::string expected;
    };

    const std::array tests {
        op {
            "-a * b",
            "((-a) * b)",
        },
        op {
            "!-a",
            "(!(-a))",
        },
        op {
            "a + b % c",
            "(a + (b % c))",
        },
        op {
            "a + b + c",
            "((a + b) + c)",
        },
        op {
            "a + b - c",
            "((a + b) - c)",
        },
        op {
            "a + b && c",
            "((a + b) && c)",
        },
        op {
            "a && b + c",
            "(a && (b + c))",
        },
        op {
            "a && b || c",
            "((a && b) || c)",
        },
        op {
            "a || b && c",
            "(a || (b && c))",
        },
        op {
            "a + b & c",
            "((a + b) & c)",
        },
        op {
            "a & b | c",
            "((a & b) | c)",
        },
        op {
            "a | b & c",
            "(a | (b & c))",
        },
        op {
            "a ^ b | c",
            "((a ^ b) | c)",
        },
        op {
            "a | b ^ c",
            "(a | (b ^ c))",
        },
        op {
            "a ^ b >> c",
            "(a ^ (b >> c))",
        },
        op {
            "a ^ b << c",
            "(a ^ (b << c))",
        },
        op {
            "a + b | c",
            "((a + b) | c)",
        },
        op {
            "a | b + c",
            "(a | (b + c))",
        },
        op {
            "a * b * c",
            "((a * b) * c)",
        },
        op {
            "a % b % c",
            "((a % b) % c)",
        },
        op {
            "a * b / c",
            "((a * b) / c)",
        },
        op {
            "a + b / c",
            "(a + (b / c))",
        },
        op {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        op {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        op {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        op {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        op {
            "5 < 4 <= 3 > 4",
            "((5 < 4) <= (3 > 4))",
        },
        op {
            "5 < 4 >= 3 > 4",
            "((5 < 4) >= (3 > 4))",
        },
        op {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        op {
            "true",
            "true",
        },
        op {
            "false",
            "false",
        },
        op {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        op {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        op {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        op {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        op {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        op {
            "(5 + 5) * 2 * (5 + 5)",
            "(((5 + 5) * 2) * (5 + 5))",
        },
        op {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        op {
            "!(true == true)",
            "(!(true == true))",
        },
        op {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)",
        },
        op {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        op {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))",
        },
        op {
            "a * [1, 2, 3, 4][b * c] * d",
            "((a * ([1, 2, 3, 4][(b * c)])) * d)",
        },
        op {
            "add(a * b[2], b[1], 2 * [1, 2][1])",
            "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))",
        },
    };

    for (const auto& test : tests) {
        auto [prgrm, _] = check_program(test.input);
        INFO("with input: ", test.input);
        CHECK(test.expected == prgrm->string());
    }
}

TEST_CASE("ifExpression")
{
    const char* input = "if (x < y) { x }";
    auto [prgrm, _] = check_program(input);
    auto* if_expr = require_expression<if_expression>(prgrm);
    require_binary_expression(if_expr->condition, "x", token_type::less_than, "y");
    REQUIRE(if_expr->consequence);
    REQUIRE_EQ(if_expr->consequence->statements.size(), 1);

    auto* consequence = dynamic_cast<const expression_statement*>(if_expr->consequence->statements[0]);
    REQUIRE(consequence);
    require_identifier(consequence->expr, "x");
    CHECK_FALSE(if_expr->alternative);
}

TEST_CASE("ifElseExpression")
{
    const char* input = "if (x < y) { x } else { y }";
    auto [prgrm, _] = check_program(input);
    auto* if_expr = require_expression<if_expression>(prgrm);

    require_binary_expression(if_expr->condition, "x", token_type::less_than, "y");
    REQUIRE(if_expr->consequence);
    REQUIRE_EQ(if_expr->consequence->statements.size(), 1);

    auto* consequence = dynamic_cast<const expression_statement*>(if_expr->consequence->statements[0]);
    REQUIRE(consequence);
    require_identifier(consequence->expr, "x");

    auto* alternative = dynamic_cast<const expression_statement*>(if_expr->alternative->statements[0]);
    REQUIRE(alternative);
    require_identifier(alternative->expr, "y");
}

TEST_CASE("whileStatement")
{
    const char* input = "while(x < y) { x = x + 1 }";
    auto [prgrm, _] = check_program(input);
    auto* while_stmt = require_statement<while_statement>(prgrm);

    require_binary_expression(while_stmt->condition, "x", token_type::less_than, "y");

    REQUIRE(while_stmt->body);
    REQUIRE_EQ(while_stmt->body->statements.size(), 1);
    const auto* assign_expr = require_assign_expression(while_stmt->body->statements[0], "x");
    require_binary_expression(assign_expr->value, "x", token_type::plus, 1);
}

TEST_CASE("breakStatement")
{
    using namespace std::string_view_literals;
    constexpr std::array tests {
        "break"sv,
        "break;"sv,
    };

    for (const auto input : tests) {
        auto [prgrm, _] = check_program(input);
        require_statement<break_statement>(prgrm);
    }
}

TEST_CASE("continueStatement")
{
    using namespace std::string_view_literals;
    constexpr std::array tests {
        "continue"sv,
        "continue;"sv,
    };

    for (const auto input : tests) {
        auto [prgrm, _] = check_program(input);
        require_statement<continue_statement>(prgrm);
    }
}

TEST_CASE("functionLiteral")
{
    const char* input = "fn(x, y) { x + y; }";
    auto [prgrm, _] = check_program(input);
    auto* fn_expr = require_expression<function_literal>(prgrm);

    REQUIRE_EQ(fn_expr->parameters.size(), 2);

    REQUIRE_EQ(fn_expr->parameters[0]->value, "x");
    REQUIRE_EQ(fn_expr->parameters[1]->value, "y");

    auto* block = dynamic_cast<const block_statement*>(fn_expr->body);
    REQUIRE_EQ(block->statements.size(), 1);

    auto* body_stmt = dynamic_cast<const expression_statement*>(block->statements[0]);
    REQUIRE(body_stmt);
    require_binary_expression(body_stmt->expr, "x", token_type::plus, "y");
}

TEST_CASE("functionLiteralWithName")
{
    const auto* input = R"(let myFunction = fn() { };)";
    auto [prgrm, _] = check_program(input);
    auto* let = require_let_statement(prgrm->statements[0], "myFunction");
    auto* fnexpr = dynamic_cast<const function_literal*>(let->value);
    REQUIRE(fnexpr);
    REQUIRE_EQ(fnexpr->name, "myFunction");
}

TEST_CASE("functionParameters")
{
    struct pt
    {
        std::string_view input;
        std::vector<std::string> expected;
    };

    std::array parameter_tests {
        pt {"fn() {};", {}},
        pt {"fn(x) {};", {"x"}},
        pt {"fn(x, y, z) {};", {"x", "y", "z"}},
    };
    for (const auto& [input, expected] : parameter_tests) {
        auto [prgrm, _] = check_program(input);
        auto* fn_expr = require_expression<function_literal>(prgrm);

        REQUIRE_EQ(fn_expr->parameters.size(), expected.size());
        for (size_t index = 0; const auto& val : expected) {
            REQUIRE_EQ(fn_expr->parameters[index]->value, val);
            ++index;
        }
    }
}

TEST_CASE("callExpressionParsing")
{
    const auto* input = "add(1, 2 * 3, 4 + 5);";
    auto [prgrm, _] = check_program(input);
    auto* call = require_expression<call_expression>(prgrm);
    require_identifier(call->function, "add");
    REQUIRE_EQ(call->arguments.size(), 3);
    require_literal_expression(call->arguments[0], 1);
    require_binary_expression(call->arguments[1], 2, token_type::asterisk, 3);
    require_binary_expression(call->arguments[2], 4, token_type::plus, 5);
}

TEST_CASE("stringLiteralExpression")
{
    const auto* input = R"("hello world";)";
    auto [prgrm, _] = check_program(input);
    auto* str = require_expression<string_literal>(prgrm);
    REQUIRE_EQ(str->value, "hello world");
}

TEST_CASE("arrayExpression")
{
    auto [prgrm, _] = check_program("[1, 2 * 2, 3 + 3]");
    auto* array_expr = require_expression<array_literal>(prgrm);
    REQUIRE_EQ(array_expr->elements.size(), 3);
    require_integer_literal(array_expr->elements[0], 1);
    require_binary_expression(array_expr->elements[1], 2, token_type::asterisk, 2);
    require_binary_expression(array_expr->elements[2], 3, token_type::plus, 3);
}

TEST_CASE("indexExpression")
{
    auto [prgrm, _] = check_program("myArray[1+1]");
    auto* idx_expr = require_expression<index_expression>(prgrm);
    require_identifier(idx_expr->left, "myArray");
    require_binary_expression(idx_expr->index, 1, token_type::plus, 1);
}

TEST_CASE("hashLiteralStringKeys")
{
    auto [prgrm, _] = check_program(R"({"one": 1, "two": 2, "three": 3})");
    auto* hash_lit = require_expression<hash_literal>(prgrm);
    for (auto idx = 0UL; const auto& [k, v] : hash_lit->pairs) {
        constexpr std::array values {1, 2, 3};
        constexpr std::array keys {"one", "two", "three"};
        require_string_literal(k, keys[idx]);
        require_integer_literal(v, values[idx]);
        idx++;
    }
}

TEST_CASE("hashLiteralWithExpression")
{
    auto [prgrm, _] = check_program(R"({"one": 0 + 1, "two": 10 - 8, "three": 15 / 5})");
    auto* hash_lit = require_expression<hash_literal>(prgrm);

    struct test
    {
        int64_t left;
        token_type oper;
        int64_t right;
    };

    constexpr std::array expected {
        test {0, token_type::plus, 1},
        test {10, token_type::minus, 8},
        test {15, token_type::slash, 5},
    };
    for (size_t idx = 0; const auto& [k, v] : hash_lit->pairs) {
        constexpr std::array keys {"one", "two", "three"};
        require_string_literal(k, keys[idx]);
        require_binary_expression(v, expected[idx].left, expected[idx].oper, expected[idx].right);
        idx++;
    }
}

TEST_CASE("emptyHashLiteral")
{
    auto [prgrm, _] = check_program(R"({})");
    auto* hash_lit = require_expression<hash_literal>(prgrm);
    REQUIRE(hash_lit->pairs.empty());
    REQUIRE(hash_lit->loc() == location {"<stdin>", 1, 1});
}

TEST_CASE("nullLiteral")
{
    auto [prgrm, _] = check_program(R"(null)");
    auto* hash_lit = require_expression<null_literal>(prgrm);
}

TEST_SUITE_END();
// NOLINTEND(*)
}  // namespace
