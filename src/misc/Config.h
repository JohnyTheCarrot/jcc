#ifndef CONFIG_H
#define CONFIG_H

#include <istream>
#include <regex>
#include <string>

#define TK_WIDE

#ifdef TK_WIDE
#define C(c) L##c
using Char = wchar_t;
#else
#define C(c) c
using Char = char;
#endif
constexpr auto EOFChar = std::char_traits<Char>::eof();

using IStream      = std::basic_istream<Char>;
using IFStream     = std::basic_ifstream<Char>;
using ISStream     = std::basic_istringstream<Char>;
using OStream      = std::basic_ostream<Char>;
using String       = std::basic_string<Char>;
using StringView   = std::basic_string_view<Char>;
using StringStream = std::basic_stringstream<Char>;
using Regex        = std::basic_regex<Char>;

#endif//CONFIG_H
