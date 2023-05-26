#include <cctype>
#include <string_view>

#include "lib.hpp"

inline auto is_letter(char chr) -> bool
{
    return std::isalpha(static_cast<unsigned char>(chr)) != 0;
}

inline auto is_digit(char chr) -> bool
{
    return std::isdigit(static_cast<unsigned char>(chr)) != 0;
}
lexxur::lexxur(std::string_view contents)
    : m_contents {contents}
{
}

auto lexxur::next_token() -> token
{
    if (!can_peek()) {
        return token {token_type::unknown, ""};
    }
    skip_whitespace();
    if (!can_peek()) {
        return token {token_type::unknown, ""};
    }
    switch (peek_char()) {
        case '&':
            next_char();
            return token {token_type::ampersand, "&"};
        case '*':
            next_char();
            return token {token_type::asterisk, "*"};
        case '^':
            next_char();
            return token {token_type::caret, "^"};
        case '}':
            next_char();
            return token {token_type::close_squirly, "}"};
        case ']':
            next_char();
            return token {token_type::close_bracket, "]"};
        case ')':
            next_char();
            return token {token_type::close_paren, ")"};
        case ':':
            next_char();
            return token {token_type::colon, ":"};
        case ',':
            next_char();
            return token {token_type::comma, ","};
        case '=':
            next_char();
            return token {token_type::equal, "="};
        case '>':
            next_char();
            return token {token_type::greater_than, ">"};
        case '<':
            next_char();
            return token {token_type::less_than, "<"};
        case '{':
            next_char();
            return token {token_type::open_squirly, "{"};
        case '[':
            next_char();
            return token {token_type::open_bracket, "["};
        case '(':
            next_char();
            return token {token_type::open_paren, "("};
        case ';':
            next_char();
            return token {token_type::semicolon, ";"};
        case '.':
            next_char();
            return token {token_type::dot, "."};
        case '/':
            next_char();
            return token {token_type::slash, "/"};
        case '\\':
            next_char();
            return token {token_type::back_slash, "\\"};
        case '-':
            next_char();
            return token {token_type::minus, "-"};
        case '+':
            next_char();
            return token {token_type::plus, "+"};
        case '|':
            next_char();
            return token {token_type::pipe, "|"};
        case '%':
            next_char();
            return token {token_type::percent, "%"};
        case '~':
            next_char();
            return token {token_type::tilde, "~"};
        case '!':
            next_char();
            return token {token_type::exclamation, "!"};
        case '?':
            next_char();
            return token {token_type::question, "?"};
    }
    if (is_letter(peek_char())) {
        return read_identifier();
    }
    if (is_digit(peek_char())) {
        return read_integer();
    }
    return token {token_type::unknown, ""};
}

auto lexxur::next_char() -> std::optional<std::string_view::value_type>
{
    m_position++;
    if (can_peek()) {
        return peek_char();
    }
    return std::nullopt;
}

auto lexxur::skip_whitespace() -> void
{
    if (!can_peek()) {
        return;
    }
    auto chur = peek_char();
    while (chur == ' ' || chur == '\t' || chur == '\n' || chur == '\r') {
        next_char();
        if (!can_peek()) {
            return;
        }
        chur = peek_char();
    }
}

auto lexxur::peek_char() -> std::string_view::value_type
{
    return m_contents.at(m_position);
}

auto lexxur::read_identifier() -> token
{
    const auto pos = m_position;
    while (can_peek() && is_letter(peek_char())) {
        next_char();
    }
    auto end = m_position;
    auto count = can_peek() ? end - pos : std::string_view::npos;
    return token {token_type::identifier, m_contents.substr(pos, count)};
}

auto lexxur::read_integer() -> token
{
    const auto pos = m_position;
    while (can_peek() && is_digit(peek_char())) {
        next_char();
    }
    auto end = m_position;
    auto count = can_peek() ? end - pos : std::string_view::npos;
    return token {token_type::integer, m_contents.substr(pos, count)};
}

auto lexxur::can_peek() -> bool
{
    return m_position + 1 <= m_contents.size();
}
