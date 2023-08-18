#include <cstdint>
#include <string>
#include <string_view>

#include <compiler/compiler.hpp>
#include <compiler/vm.hpp>
#include <eval/object.hpp>
#include <gtest/gtest.h>

#include "testutils.hpp"

auto assert_bool_object(bool expected, const object& actual, std::string_view input) -> void
{
    ASSERT_TRUE(actual.is<bool>()) << input << " got " << actual.type_name() << " with " << std::to_string(actual.value)
                                   << " instead";
    auto actual_bool = actual.as<bool>();
    ASSERT_EQ(actual_bool, expected);
}

auto assert_integer_object(int64_t expected, const object& actual, std::string_view input) -> void
{
    ASSERT_TRUE(actual.is<integer_value>())
        << input << " got " << actual.type_name() << " with " << std::to_string(actual.value) << " instead ";
    auto actual_int = actual.as<integer_value>();
    ASSERT_EQ(actual_int, expected);
}

auto assert_string_object(const std::string& expected, const object& actual, std::string_view input) -> void
{
    ASSERT_TRUE(actual.is<string_value>())
        << input << " got " << actual.type_name() << " with " << std::to_string(actual.value) << " instead ";
    auto actual_str = actual.as<string_value>();
    ASSERT_EQ(actual_str, expected);
}

auto assert_array_object(const std::vector<int>& expected, const object& actual, std::string_view input) -> void
{
    ASSERT_TRUE(actual.is<array>()) << input << " got " << actual.type_name() << " with "
                                    << std::to_string(actual.value) << " instead ";
    auto actual_arr = actual.as<array>();
    ASSERT_EQ(actual_arr.size(), expected.size());
    for (auto idx = 0UL; const auto& elem : expected) {
        ASSERT_EQ(elem, actual_arr.at(idx).as<integer_value>());
        ++idx;
    }
}

auto assert_hash_object(const hash& expected, const object& actual, std::string_view input) -> void
{
    ASSERT_TRUE(actual.is<hash>()) << input << " got " << actual.type_name() << " with " << std::to_string(actual.value)
                                   << " instead ";
    auto actual_hash = actual.as<hash>();
    ASSERT_EQ(actual_hash.size(), expected.size());
}

template<typename... T>
struct vt
{
    std::string_view input;
    std::variant<T...> expected;
};

template<typename... T>
auto assert_expected_object(const std::variant<T...>& expected, const object& actual, std::string_view input) -> void
{
    std::visit(
        overloaded {
            [&](const int64_t exp) { assert_integer_object(exp, actual, input); },
            [&](const bool exp) { assert_bool_object(exp, actual, input); },
            [&](const nil_value) { ASSERT_TRUE(actual.is_nil()); },
            [&](const std::string& exp) { assert_string_object(exp, actual, input); },
            [&](const std::vector<int>& exp) { assert_array_object(exp, actual, input); },
            [&](const hash& exp) { assert_hash_object(exp, actual, input); },
        },
        expected);
}

template<size_t N, typename... Expecteds>
auto rvt(std::array<vt<Expecteds...>, N> tests)
{
    for (const auto& [input, expected] : tests) {
        auto [prgrm, _] = assert_program(input);
        auto cmplr = compiler::create();
        cmplr.compile(prgrm);
        auto vm = vm::create(std::move(cmplr.code));
        vm.run();

        auto top = vm.last_popped();
        assert_expected_object(expected, top, input);
    }
}

// NOLINTBEGIN(*-magic-numbers)
TEST(vm, integerArithmetics)
{
    std::array tests {
        vt<int64_t> {"1", 1},
        vt<int64_t> {"2", 2},
        vt<int64_t> {"1 + 2", 3},
        vt<int64_t> {"1 - 2", -1},
        vt<int64_t> {"1 * 2", 2},

        vt<int64_t> {"4 / 2", 2},
        vt<int64_t> {"50 / 2 * 2 + 10 - 5", 55},
        vt<int64_t> {"5 + 5 + 5 + 5 - 10", 10},
        vt<int64_t> {"2 * 2 * 2 * 2 * 2", 32},
        vt<int64_t> {"5 * 2 + 10", 20},
        vt<int64_t> {"5 + 2 * 10", 25},
        vt<int64_t> {"5 * (2 + 10)", 60},
        vt<int64_t> {"-5", -5},
        vt<int64_t> {"-10", -10},
        vt<int64_t> {"-50 + 100 + -50", 0},
        vt<int64_t> {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };
    rvt(tests);
}

TEST(vm, booleanExpressions)
{
    std::array tests {
        vt<bool> {"true", true},
        vt<bool> {"false", false},
        vt<bool> {"1 < 2", true},
        vt<bool> {"1 > 2", false},
        vt<bool> {"1 < 1", false},
        vt<bool> {"1 > 1", false},
        vt<bool> {"1 == 1", true},
        vt<bool> {"1 != 1", false},
        vt<bool> {"1 == 2", false},
        vt<bool> {"1 != 2", true},
        vt<bool> {"true == true", true},
        vt<bool> {"false == false", true},
        vt<bool> {"true == false", false},
        vt<bool> {"true != false", true},
        vt<bool> {"false != true", true},
        vt<bool> {"(1 < 2) == true", true},
        vt<bool> {"(1 < 2) == false", false},
        vt<bool> {"(1 > 2) == true", false},
        vt<bool> {"(1 > 2) == false", true},
        vt<bool> {"!true", false},
        vt<bool> {"!false", true},
        vt<bool> {"!5", false},
        vt<bool> {"!!true", true},
        vt<bool> {"!!false", false},
        vt<bool> {"!!5", true},
        vt<bool> {"!(if (false) { 5; })", true},
    };
    rvt(tests);
}

TEST(vm, conditionals)
{
    std::array tests {
        vt<int64_t, nil_value> {"if (true) { 10 }", 10},
        vt<int64_t, nil_value> {"if (true) { 10 } else { 20 }", 10},
        vt<int64_t, nil_value> {"if (false) { 10 } else { 20 } ", 20},
        vt<int64_t, nil_value> {"if (1) { 10 }", 10},
        vt<int64_t, nil_value> {"if (1 < 2) { 10 }", 10},
        vt<int64_t, nil_value> {"if (1 < 2) { 10 } else { 20 }", 10},
        vt<int64_t, nil_value> {"if (1 > 2) { 10 } else { 20 }", 20},
        vt<int64_t, nil_value> {"if (1 > 2) { 10 }", nil_value {}},
        vt<int64_t, nil_value> {"if (false) { 10 }", nil_value {}},
        vt<int64_t, nil_value> {"if ((if (false) { 10 })) { 10 } else { 20 }", 20},
    };
    rvt(tests);
}

TEST(vm, globalLetStatemets)
{
    std::array tests {
        vt<int64_t> {"let one = 1; one", 1},
        vt<int64_t> {"let one = 1; let two = 2; one + two", 3},
        vt<int64_t> {"let one = 1; let two = one + one; one + two", 3},
    };
    rvt(tests);
}

TEST(vm, stringExpression)
{
    std::array tests {
        vt<std::string> {R"("monkey")", "monkey"},
        vt<std::string> {R"("mon" + "key")", "monkey"},
        vt<std::string> {R"("mon" + "key" + "banana")", "monkeybanana"},

    };
    rvt(tests);
}

TEST(vm, arrayLiterals)
{
    std::array tests {
        vt<std::vector<int>> {
            "[]",
            {},
        },
        vt<std::vector<int>> {
            "[1, 2, 3]",
            {std::vector<int> {1, 2, 3}},
        },
        vt<std::vector<int>> {
            "[1+ 2, 3 * 4, 5 + 6]",
            {std::vector<int> {3, 12, 11}},
        },
    };
    rvt(tests);
}

TEST(vm, hashLiterals)
{
    std::array tests {
        vt<hash> {
            "{}",
            {},
        },
        vt<hash> {
            "{1: 2, 3: 4}",
            {hash {{1, 2}, {3, 4}}},
        },
        vt<hash> {
            "{1 + 1: 2 * 2, 3 + 3: 4 * 4}",
            {hash {{2, 4}, {6, 16}}},

        },
    };
    rvt(tests);
}

TEST(vm, indexExpressions)
{
    std::array tests {
        vt<int64_t, nil_value> {"[1, 2, 3][1]", 2},
        vt<int64_t, nil_value> {"[1, 2, 3][0 + 2]", 3},
        vt<int64_t, nil_value> {"[[1, 1, 1]][0][0]", 1},
        vt<int64_t, nil_value> {"[][0]", nil_value {}},
        vt<int64_t, nil_value> {"[1, 2, 3][99]", nil_value {}},
        vt<int64_t, nil_value> {"[1][-1]", nil_value {}},
        vt<int64_t, nil_value> {"{1: 1, 2: 2}[1]", 1},
        vt<int64_t, nil_value> {"{1: 1, 2: 2}[2]", 2},
        vt<int64_t, nil_value> {"{1: 1}[0]", nil_value {}},
        vt<int64_t, nil_value> {"{}[0]", nil_value {}},
    };
    rvt(tests);
}
// NOLINTEND(*-magic-numbers)
