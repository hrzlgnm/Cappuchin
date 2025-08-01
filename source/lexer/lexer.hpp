#pragma once
#include <string_view>

#include "token.hpp"

class lexer final
{
  public:
    explicit lexer(std::string_view input, std::string_view filename = "<stdin>");

    auto next_token() -> token;

  private:
    auto read_char() -> void;
    auto skip_whitespace() -> void;
    auto peek_char() const -> std::string_view::value_type;
    auto read_identifier_or_keyword() -> token;
    auto read_number() -> token;
    auto read_string() -> token;
    auto current_loc() const -> location;

    std::string_view m_input;
    std::string_view m_filename;
    std::string_view::size_type m_position {0};
    std::string_view::size_type m_bol {0};
    std::string_view::size_type m_row {0};
    std::string_view::size_type m_read_position {0};
    std::string_view::value_type m_byte {0};
};
