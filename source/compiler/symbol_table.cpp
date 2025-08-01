#include <array>
#include <map>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "symbol_table.hpp"

#include <doctest/doctest.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/std.h>  // for formatting std::optional
#include <gc.hpp>

auto operator==(const symbol_pointer& lhs, const symbol_pointer& rhs) -> bool
{
    return lhs.index == rhs.index && lhs.scope == rhs.scope && lhs.level == rhs.level;
}

auto operator==(const symbol& lhs, const symbol& rhs) -> bool
{
    return lhs.name == rhs.name && lhs.scope == rhs.scope && lhs.index == rhs.index && lhs.ptr == rhs.ptr;
}

auto operator<<(std::ostream& ost, const symbol_scope scope) -> std::ostream&
{
    using enum symbol_scope;
    switch (scope) {
        case global:
            return ost << "global";
        case local:
            return ost << "local";
        case builtin:
            return ost << "builtin";
        case free:
            return ost << "free";
        case function:
            return ost << "function";
        case outer:
            return ost << "outer";
    }
    return ost;
}

auto operator<<(std::ostream& ost, const symbol& sym) -> std::ostream&
{
    return ost << fmt::format("symbol{{{}, {}, {}, {}}}", sym.name, sym.scope, sym.index, sym.ptr);
}

auto operator<<(std::ostream& ost, const symbol_pointer& ptr) -> std::ostream&
{
    return ost << fmt::format("symbol_pointer{{{}, {}, {}}}", ptr.level, ptr.scope, ptr.index);
}

auto symbol_table::create() -> symbol_table*
{
    return allocate<symbol_table>();
}

auto symbol_table::create_enclosed(symbol_table* outer, bool inside_loop) -> symbol_table*
{
    return allocate<symbol_table>(outer, inside_loop);
}

symbol_table::symbol_table(symbol_table* outer, const bool inside_loop)
    : m_outer {outer}
    , m_inside_loop {inside_loop}
{
}

auto symbol_table::define(const std::string& name) -> symbol
{
    using enum symbol_scope;
    return m_store[name] = symbol {
               .name = name,
               .scope = m_outer != nullptr ? local : global,
               .index = m_defs++,
           };
}

auto symbol_table::define_builtin(const int index, const std::string& name) -> symbol
{
    return m_store[name] = symbol {
               .name = name,
               .scope = symbol_scope::builtin,
               .index = index,
           };
}

auto symbol_table::define_function_name(const std::string& name) -> symbol
{
    return m_store[name] = symbol {
               .name = name,
               .scope = symbol_scope::function,
               .index = 0,
           };
}

auto symbol_table::define_outer(const symbol& original, const int level) -> symbol
{
    using enum symbol_scope;
    if (original.scope == local || original.scope == free || original.scope == function) {
        return m_store[original.name] = symbol {.name = original.name,
                                                .scope = outer,
                                                .index = 0,
                                                .ptr =
                                                    symbol_pointer {
                                                        .level = level,
                                                        .scope = original.scope,
                                                        .index = original.index,
                                                    }

               };
    }
    if (original.scope == outer) {
        return m_store[original.name] = symbol {.name = original.name,
                                                .scope = outer,
                                                .index = 0,
                                                .ptr = symbol_pointer {
                                                    .level = level + original.ptr->level,
                                                    .scope = original.ptr->scope,
                                                    .index = original.ptr->index,
                                                }};
    }
    throw std::runtime_error("invalid call to define_outer");
}

auto symbol_table::resolve(const std::string& name, int level) -> std::optional<symbol>
{
    using enum symbol_scope;
    if (const auto itr = m_store.find(name); itr != m_store.end()) {
        return itr->second;
    }
    if (m_outer != nullptr) {
        level = level + 1;
        auto maybe_symbol = m_outer->resolve(name, level);
        if (!maybe_symbol.has_value()) {
            return maybe_symbol;
        }
        auto symbol = maybe_symbol.value();
        if (symbol.scope == global || symbol.scope == builtin) {
            return symbol;
        }
        if (m_inside_loop) {
            return define_outer(symbol, level);
        }
        return define_free(symbol);
    }
    return std::nullopt;
}

auto symbol_table::free() const -> const std::vector<symbol>&
{
    return m_free;
}

auto symbol_table::define_free(const symbol& sym) -> symbol
{
    m_free.push_back(sym);
    return m_store[sym.name] = symbol {
               .name = sym.name,
               .scope = symbol_scope::free,
               .index = static_cast<int>(m_free.size()) - 1,
           };
}

auto symbol_table::debug() const -> void
{
    for (const auto& [name, symbol] : m_store) {
        fmt::println("{}", symbol);
    }
    if (m_outer != nullptr) {
        fmt::println("Outer:");
        m_outer->debug();
    }
}

namespace
{
// NOLINTBEGIN(*)
TEST_SUITE_BEGIN("symbol table");

TEST_CASE("define")
{
    using enum symbol_scope;
    auto expected = string_map<symbol> {
        {"a", symbol {"a", global, 0, std::nullopt}},
        {"b", symbol {"b", global, 1, std::nullopt}},
        {"c", symbol {"c", local, 0, std::nullopt}},
        {"d", symbol {"d", local, 1, std::nullopt}},
        {"e", symbol {"e", local, 0, std::nullopt}},
        {"f", symbol {"f", local, 1, std::nullopt}},
    };

    auto globals = symbol_table::create();
    auto a = globals->define("a");
    CHECK_EQ(a, expected["a"]);
    auto b = globals->define("b");
    CHECK_EQ(b, expected["b"]);

    auto first = symbol_table::create_enclosed(globals);
    auto c = first->define("c");
    CHECK_EQ(c, expected["c"]);
    auto d = first->define("d");
    CHECK_EQ(d, expected["d"]);

    auto second = symbol_table::create_enclosed(first);
    auto e = second->define("e");
    CHECK_EQ(e, expected["e"]);
    auto f = second->define("f");
    CHECK_EQ(f, expected["f"]);
}

TEST_CASE("resolve")
{
    auto globals = symbol_table::create();
    globals->define("a");
    globals->define("b");

    using enum symbol_scope;
    std::array expecteds {
        symbol {"a", global, 0, std::nullopt},
        symbol {"b", global, 1, std::nullopt},
    };

    for (const auto& expected : expecteds) {
        CHECK_EQ(globals->resolve(expected.name), expected);
    }

    SUBCASE("resolveLocal")
    {
        auto locals = symbol_table::create_enclosed(globals);
        locals->define("c");
        locals->define("d");

        using enum symbol_scope;
        const std::array expecteds {
            symbol {"a", global, 0, std::nullopt},
            symbol {"b", global, 1, std::nullopt},
            symbol {"c", local, 0, std::nullopt},
            symbol {"d", local, 1, std::nullopt},
        };
        for (const auto& expected : expecteds) {
            CHECK_EQ(locals->resolve(expected.name), expected);
        }

        SUBCASE("resolveNestedLocals")
        {
            auto nested = symbol_table::create_enclosed(locals);
            nested->define("e");
            nested->define("f");

            using enum symbol_scope;
            std::array expecteds {
                symbol {"a", global, 0, std::nullopt},
                symbol {"b", global, 1, std::nullopt},
                symbol {"c", free, 0, std::nullopt},
                symbol {"d", free, 1, std::nullopt},
                symbol {"e", local, 0, std::nullopt},
                symbol {"f", local, 1, std::nullopt},
            };
            for (const auto& expected : expecteds) {
                CHECK_EQ(nested->resolve(expected.name), expected);
            }

            SUBCASE("resolveFree")
            {
                REQUIRE_EQ(nested->free().size(), 2);
                CHECK_EQ(nested->free()[0].name, "c");
                CHECK_EQ(nested->free()[1].name, "d");
            }
        }
    }
}

TEST_CASE("defineResolveBuiltin")
{
    using enum symbol_scope;
    std::array expecteds {
        symbol {"a", builtin, 0, std::nullopt},
        symbol {"c", builtin, 1, std::nullopt},
        symbol {"e", builtin, 2, std::nullopt},
        symbol {"f", builtin, 3, std::nullopt},
    };

    auto globals = symbol_table::create();
    auto first = symbol_table::create_enclosed(globals);
    auto nested = symbol_table::create_enclosed(first);
    for (auto i = 0; const auto& expected : expecteds) {
        globals->define_builtin(i, expected.name);
        i++;
    }

    for (const auto& table : {globals, first, nested}) {
        for (const auto& expected : expecteds) {
            CHECK_EQ(table->resolve(expected.name), expected);
        }
    }
}

TEST_CASE("defineAndResolveFunctionName")
{
    using enum symbol_scope;
    auto globals = symbol_table::create();
    globals->define_function_name("a");

    auto expected = symbol {"a", function, 0, std::nullopt};

    auto actual = globals->resolve("a");
    REQUIRE(actual.has_value());
    REQUIRE_EQ(expected, actual.value());
}

TEST_CASE("shadowFunctionNames")
{
    using enum symbol_scope;
    auto globals = symbol_table::create();
    globals->define_function_name("a");
    globals->define("a");

    auto expected = symbol {"a", global, 0, std::nullopt};
    auto resolved = globals->resolve("a");
    REQUIRE(resolved.has_value());
    REQUIRE_EQ(resolved.value(), expected);
}

TEST_SUITE_END();
// NOLINTEND(*)
}  // namespace
