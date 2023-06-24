
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "lexer.hpp"

#include <gtest/gtest.h>

#include "ast.hpp"
#include "parser.hpp"
#include "token_type.hpp"

// NOLINTBEGIN
using value_type = std::variant<int64_t, std::string, bool>;

template<class>
inline constexpr bool always_false_v = false;

inline auto assert_boolean_literal(const expression_ptr& expr, bool value)
    -> void
{
    auto* bool_expr = dynamic_cast<boolean*>(expr.get());
    ASSERT_TRUE(bool_expr);
    ASSERT_EQ(bool_expr->value, value);
}

inline auto assert_identifier(const expression_ptr& expr,
                              const std::string& value) -> void
{
    auto* ident = dynamic_cast<identifier*>(expr.get());
    ASSERT_TRUE(ident);
    ASSERT_EQ(ident->value, value);
}

inline auto assert_integer_literal(const expression_ptr& expr, int64_t value)
    -> void
{
    auto* integer_lit = dynamic_cast<integer_literal*>(expr.get());
    ASSERT_TRUE(integer_lit);

    ASSERT_EQ(integer_lit->value, value);
    ASSERT_EQ(integer_lit->token_literal(), std::to_string(value));
}

inline void assert_literal_expression(const expression_ptr& expr,
                                      const value_type& expected)
{
    std::visit(
        [&](auto&& arg)
        {
            using t = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<t, int64_t>) {
                assert_integer_literal(expr, arg);
            } else if constexpr (std::is_same_v<t, std::string>) {
                assert_identifier(expr, arg);
            } else if constexpr (std::is_same_v<t, bool>) {
                assert_boolean_literal(expr, arg);
            } else {
                static_assert(always_false_v<t>, "non-exhaustive visitor!");
            }
        },
        expected);
}

auto assert_infix_expression(const expression_ptr& expr,
                             const value_type& left,
                             const std::string& oprtr,
                             const value_type& right) -> void
{
    auto* infix = dynamic_cast<infix_expression*>(expr.get());
    ASSERT_TRUE(infix);
    assert_literal_expression(infix->left, left);
    ASSERT_EQ(infix->op, oprtr);
    assert_literal_expression(infix->right, right);
}

auto assert_no_parse_errors(const parser& prsr)
{
    ASSERT_TRUE(prsr.errors().empty()) << testing::PrintToString(prsr.errors());
}

auto assert_expression_statement(parser& prsr, const program_ptr& prgrm)
    -> expression_statement*
{
    assert_no_parse_errors(prsr);
    if (prgrm->statements.size() != 1) {
        throw std::invalid_argument("expected one statement, got "
                                    + std::to_string(prgrm->statements.size()));
    }
    auto* stmt = prgrm->statements[0].get();
    auto expr_stmt = dynamic_cast<expression_statement*>(stmt);
    if (!expr_stmt) {
        throw std::invalid_argument("expected expression_statement, got "
                                    + stmt->string());
    }
    return expr_stmt;
}

TEST(test, lexing)
{
    using enum token_type;
    auto lxr = lexer {R"r(let five = 5;
let ten = 10;
let add = fn(x, y) {
x + y;
};
let result = add(five, ten);
)r"};
    auto expected_tokens = std::vector<token> {
        token {let, "let"},      token {ident, "five"},  token {assign, "="},
        token {integer, "5"},    token {semicolon, ";"}, token {let, "let"},
        token {ident, "ten"},    token {assign, "="},    token {integer, "10"},
        token {semicolon, ";"},  token {let, "let"},     token {ident, "add"},
        token {assign, "="},     token {function, "fn"}, token {lparen, "("},
        token {ident, "x"},      token {comma, ","},     token {ident, "y"},
        token {rparen, ")"},     token {lsquirly, "{"},  token {ident, "x"},
        token {plus, "+"},       token {ident, "y"},     token {semicolon, ";"},
        token {rsquirly, "}"},   token {semicolon, ";"}, token {let, "let"},
        token {ident, "result"}, token {assign, "="},    token {ident, "add"},
        token {lparen, "("},     token {ident, "five"},  token {comma, ","},
        token {ident, "ten"},    token {rparen, ")"},    token {semicolon, ";"},
        token {eof, ""},
    };
    for (const auto& expected_token : expected_tokens) {
        auto token = lxr.next_token();
        ASSERT_EQ(token, expected_token);
    }
}

TEST(test, lexingMoreTokens)
{
    using enum token_type;
    auto lxr = lexer {R"r(let five = 5;
let ten = 10;
let add = fn(x, y) {
x + y;
};
let result = add(five, ten);
!-/*5;
5 < 10 > 5;
if (5 < 10) {
return true;
} else {
return false;
}
10 == 10;
10 != 9;
)r"};
    auto expected_tokens = std::vector<token> {
        token {let, "let"},       token {ident, "five"},
        token {assign, "="},      token {integer, "5"},
        token {semicolon, ";"},   token {let, "let"},
        token {ident, "ten"},     token {assign, "="},
        token {integer, "10"},    token {semicolon, ";"},
        token {let, "let"},       token {ident, "add"},
        token {assign, "="},      token {function, "fn"},
        token {lparen, "("},      token {ident, "x"},
        token {comma, ","},       token {ident, "y"},
        token {rparen, ")"},      token {lsquirly, "{"},
        token {ident, "x"},       token {plus, "+"},
        token {ident, "y"},       token {semicolon, ";"},
        token {rsquirly, "}"},    token {semicolon, ";"},
        token {let, "let"},       token {ident, "result"},
        token {assign, "="},      token {ident, "add"},
        token {lparen, "("},      token {ident, "five"},
        token {comma, ","},       token {ident, "ten"},
        token {rparen, ")"},      token {semicolon, ";"},
        token {exclamation, "!"}, token {minus, "-"},
        token {slash, "/"},       token {asterisk, "*"},
        token {integer, "5"},     token {semicolon, ";"},
        token {integer, "5"},     token {less_than, "<"},
        token {integer, "10"},    token {greater_than, ">"},
        token {integer, "5"},     token {semicolon, ";"},
        token {eef, "if"},        token {lparen, "("},
        token {integer, "5"},     token {less_than, "<"},
        token {integer, "10"},    token {rparen, ")"},
        token {lsquirly, "{"},    token {ret, "return"},
        token {tru, "true"},      token {semicolon, ";"},
        token {rsquirly, "}"},    token {elze, "else"},
        token {lsquirly, "{"},    token {ret, "return"},
        token {fals, "false"},    token {semicolon, ";"},
        token {rsquirly, "}"},    token {integer, "10"},
        token {equals, "=="},     token {integer, "10"},
        token {semicolon, ";"},   token {integer, "10"},
        token {not_equals, "!="}, token {integer, "9"},
        token {semicolon, ";"},   token {eof, ""},

    };
    for (const auto& expected_token : expected_tokens) {
        auto token = lxr.next_token();
        ASSERT_EQ(token, expected_token);
    }
}

TEST(test, testLetStatement)
{
    using enum token_type;
    auto prsr = parser {lexer {
        R"r(let x = 5;
let y = 10;
let foobar = 838383;
)r"}};
    auto program = prsr.parse_program();
    ASSERT_TRUE(program);
    ASSERT_EQ(program->statements.size(), 3);
    auto expected_identifiers = std::vector<std::string> {"x", "y", "foobar"};
    for (size_t i = 0; i < 3; ++i) {
        auto* stmt = program->statements[i].get();
        auto* let_stmt = dynamic_cast<let_statement*>(stmt);
        ASSERT_TRUE(let_stmt);
        ASSERT_EQ(let_stmt->tkn.literal, "let");
        ASSERT_EQ(let_stmt->name->value, expected_identifiers[i]);
        ASSERT_EQ(let_stmt->name->tkn.literal, expected_identifiers[i]);
    }
}

TEST(test, parseError)
{
    using enum token_type;
    auto prsr = parser {lexer {
        R"r(let x = 5;
let y = 10;
let 838383;
)r"}};
    prsr.parse_program();
    auto errors = prsr.errors();
    if (errors.empty()) {
        FAIL();
    }
    for (const auto& err : errors) {
        std::cerr << "err: " << err << "\n";
    }
}

TEST(test, testReturnStatement)
{
    using enum token_type;
    auto prsr = parser {lexer {
        R"r(return 5;
return 10;
return 993322;
)r"}};
    auto program = prsr.parse_program();
    assert_no_parse_errors(prsr);
    ASSERT_TRUE(program);
    ASSERT_EQ(program->statements.size(), 3);
    for (size_t i = 0; i < 3; ++i) {
        auto* stmt = program->statements[i].get();
        auto* ret_stmt = dynamic_cast<return_statement*>(stmt);
        ASSERT_TRUE(ret_stmt);
        ASSERT_EQ(ret_stmt->tkn.literal, "return");
    }
}

TEST(test, testString)
{
    using enum token_type;
    auto ltkn = token {
        .type = let,
        .literal = "let",
    };
    auto name = std::make_unique<identifier>();
    name->tkn = token {
        .type = ident,
        .literal = "myVar",
    };
    name->value = "myVar";

    auto value = std::make_unique<identifier>();
    value->tkn = token {
        .type = ident,
        .literal = "anotherVar",
    };
    value->value = "anotherVar";
    program prgrm;

    prgrm.statements.push_back(std::make_unique<let_statement>());
    dynamic_cast<let_statement*>(prgrm.statements.back().get())->tkn = ltkn;
    dynamic_cast<let_statement*>(prgrm.statements.back().get())->name =
        std::move(name);
    dynamic_cast<let_statement*>(prgrm.statements.back().get())->value =
        std::move(value);

    ASSERT_EQ(prgrm.string(), "let myVar = anotherVar;");
}

TEST(test, testIdentfierExpression)
{
    const auto* input = "foobar;";
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    auto* expr_stmt = assert_expression_statement(prsr, prgrm);

    assert_literal_expression(expr_stmt->expr, std::string {"foobar"});
}

TEST(test, testIntegerExpression)
{
    const auto* input = "5;";
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    auto* expr_stmt = assert_expression_statement(prsr, prgrm);

    assert_literal_expression(expr_stmt->expr, 5);
}

TEST(test, testPrefixExpressions)
{
    struct prefix_test
    {
        std::string_view input;
        std::string op;
        int64_t integer_value;
    };
    std::array prefix_tests {
        // clang-format: off
        prefix_test {"!5;", "!", 5},
        prefix_test {"-15;", "-", 15}
        // clang-format: on
    };

    for (const auto& prefix_test : prefix_tests) {
        auto prsr = parser {lexer {prefix_test.input}};
        auto prgrm = prsr.parse_program();
        auto* expr_stmt = assert_expression_statement(prsr, prgrm);
        auto* expr = expr_stmt->expr.get();
        auto* prefix = dynamic_cast<prefix_expression*>(expr);
        ASSERT_TRUE(prefix);
        ASSERT_EQ(prefix_test.op, prefix->op);

        assert_literal_expression(prefix->right, prefix_test.integer_value);
    }
}

TEST(test, testInfixExpressions)
{
    struct infix_test
    {
        std::string_view input;
        int64_t left_value;
        std::string op;
        int64_t right_value;
    };
    std::array infix_tests {
        infix_test {"5 + 5;", 5, "+", 5},
        infix_test {"5 - 5;", 5, "-", 5},
        infix_test {"5 * 5;", 5, "*", 5},
        infix_test {"5 / 5;", 5, "/", 5},
        infix_test {"5 > 5;", 5, ">", 5},
        infix_test {"5 < 5;", 5, "<", 5},
        infix_test {"5 == 5;", 5, "==", 5},
        infix_test {"5 != 5;", 5, "!=", 5},
    };

    for (const auto& infix_test : infix_tests) {
        auto prsr = parser {lexer {infix_test.input}};
        auto prgrm = prsr.parse_program();
        auto* expr_stmt = assert_expression_statement(prsr, prgrm);

        assert_infix_expression(expr_stmt->expr,
                                infix_test.left_value,
                                infix_test.op,
                                infix_test.right_value);
    }
}

TEST(test, testOperatorPrecedence)
{
    struct oper_test
    {
        std::string_view input;
        std::string expected;
    };
    std::array operator_precedence_tests {
        oper_test {
            "true",
            "true",
        },
        oper_test {
            "false",
            "false",
        },
        oper_test {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        oper_test {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        oper_test {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        oper_test {

            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        oper_test {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        oper_test {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        oper_test {
            "!(true == true)",
            "(!(true == true))",
        },
    };
    for (const auto& operator_precendce_test : operator_precedence_tests) {
        auto prsr = parser {lexer {operator_precendce_test.input}};
        auto prgrm = prsr.parse_program();
        assert_no_parse_errors(prsr);
        ASSERT_EQ(prgrm->statements.size(), 1);
        auto* stmt = prgrm->statements[0].get();
        ASSERT_EQ(operator_precendce_test.expected, stmt->string());
    }
}

TEST(test, testIfExpression)
{
    const char* input = "if (x < y) { x }";
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    auto expr_stmt = assert_expression_statement(prsr, prgrm);
    auto* if_expr = dynamic_cast<if_expression*>(expr_stmt->expr.get());
    ASSERT_TRUE(if_expr);
    assert_infix_expression(if_expr->condition, "x", "<", "y");
    ASSERT_TRUE(if_expr->consequence);
    ASSERT_EQ(if_expr->consequence->statements.size(), 1);

    auto* consequence = dynamic_cast<expression_statement*>(
        if_expr->consequence->statements[0].get());
    ASSERT_TRUE(consequence);
    assert_identifier(consequence->expr, "x");
    ASSERT_FALSE(if_expr->alternative);
}

TEST(test, testIfElseExpression)
{
    const char* input = "if (x < y) { x } else { y }";
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    auto* expr_stmt = assert_expression_statement(prsr, prgrm);
    auto* if_expr = dynamic_cast<if_expression*>(expr_stmt->expr.get());
    ASSERT_TRUE(if_expr);

    assert_infix_expression(if_expr->condition, "x", "<", "y");
    ASSERT_TRUE(if_expr->consequence);
    ASSERT_EQ(if_expr->consequence->statements.size(), 1);

    auto* consequence = dynamic_cast<expression_statement*>(
        if_expr->consequence->statements[0].get());
    ASSERT_TRUE(consequence);
    assert_identifier(consequence->expr, "x");

    auto* alternative = dynamic_cast<expression_statement*>(
        if_expr->alternative->statements[0].get());
    ASSERT_TRUE(alternative);
    assert_identifier(alternative->expr, "y");
}

TEST(test, testFunctionLiteral)
{
    const char* input = "fn(x, y) { x + y; }";
    auto prsr = parser {lexer {input}};
    auto prgrm = prsr.parse_program();
    auto* expr_stmt = assert_expression_statement(prsr, prgrm);
    auto* fn_literal = dynamic_cast<function_literal*>(expr_stmt->expr.get());
    ASSERT_TRUE(fn_literal);

    ASSERT_EQ(fn_literal->parameters.size(), 2);

    assert_literal_expression(std::move(fn_literal->parameters[0]), "x");
    assert_literal_expression(std::move(fn_literal->parameters[1]), "y");

    ASSERT_EQ(fn_literal->body->statements.size(), 1);
    auto* body_stmt = dynamic_cast<expression_statement*>(
        fn_literal->body->statements.at(0).get());

    assert_infix_expression(body_stmt->expr, "x", "+", "y");
}

TEST(test, testFunctionParameters)
{
    struct parameters_test
    {
        std::string_view input;
        std::vector<std::string> expected;
    };
    std::array parameter_tests {
        parameters_test {"fn() {};", {}},
        parameters_test {"fn(x) {};", {"x"}},
        parameters_test {"fn(x, y, z) {};", {"x", "y", "z"}},
    };
    for (const auto& parameter_test : parameter_tests) {
        auto prsr = parser {lexer {parameter_test.input}};
        auto prgrm = prsr.parse_program();
        auto* expr_stmt = assert_expression_statement(prsr, prgrm);
        auto* fn_literal =
            dynamic_cast<function_literal*>(expr_stmt->expr.get());
        ASSERT_TRUE(fn_literal);
        ASSERT_EQ(fn_literal->parameters.size(),
                  parameter_test.expected.size());
        for (size_t index = 0; const auto& expected : parameter_test.expected) {
            assert_literal_expression(std::move(fn_literal->parameters[index]),
                                      expected);
            ++index;
        }
    }
}

// NOLINTEND
