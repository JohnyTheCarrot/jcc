//
// Created by tuurm on 03/07/2023.
//

#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H


#include <vector>
#include "frozen/unordered_map.h"
#include "frozen/string.h"
#include <optional>
#include <variant>
#include <memory>
#include "Span.h"

using IntegerTokenValue = int64_t;
using FloatingTokenValue = double;

enum class TokenType {
    Increment,
    Plus,
    AddAssign,
    Decrement,
    Minus,
    SubtractAssign,
    BitwiseNot,
    Asterisk,
    MultiplyAssign,
    Divide,
    DivideAssign,
    Backslash,
    Semicolon,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    LeftParenthesis,
    RightParenthesis,
    ExclusiveOr,
    ExclusiveOrAssign,
    Dot,
    Arrow,
    Comma,
    Hash,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    ShiftLeft,
    ShiftLeftAssign,
    ShiftRight,
    ShiftRightAssign,
    LogicalOr,
    BitwiseOr,
    BitwiseOrAssign,
    LogicalAnd,
    Ampersand,
    BitwiseAndAssign,
    NotEquals,
    LogicalNot,
    Assign,
    Equals,
    QuestionMark,
    Colon,
    Modulo,
    ModuloAssign,
    IntegerLiteral,
    FloatLiteral,
    DoubleLiteral,
    StringLiteral,

    // Keywords
    KeywordAuto,
    KeywordBreak,
    KeywordCase,
    KeywordChar,
    KeywordConst,
    KeywordContinue,
    KeywordDefault,
    KeywordDo,
    KeywordDouble,
    KeywordElse,
    KeywordEnum,
    KeywordExtern,
    KeywordFloat,
    KeywordFor,
    KeywordGoto,
    KeywordIf,
    KeywordInline,
    KeywordInt,
    KeywordLong,
    KeywordRegister,
    KeywordRestrict,
    KeywordReturn,
    KeywordShort,
    KeywordSigned,
    KeywordSizeof,
    KeywordStatic,
    KeywordStruct,
    KeywordSwitch,
    KeywordTypedef,
    KeywordUnion,
    KeywordUnsigned,
    KeywordVoid,
    KeywordVolatile,
    KeywordWhile,
    KeywordAlignas,
    KeywordAlignof,
    KeywordAtomic,
    KeywordBool,
    KeywordComplex,
    KeywordGeneric,
    KeywordImaginary,
    KeywordNoreturn,
    KeywordStaticAssert,
    KeywordThreadLocal,

    Identifier
};

constexpr frozen::string LINE_COMMENT{"//"};

constexpr size_t MAX_TOKEN_LENGTH{16};

constexpr frozen::unordered_map<frozen::string, TokenType, 100> TOKEN_DEFINITIONS = {
        {"+",              TokenType::Plus},
        {"+=",             TokenType::AddAssign},
        {"++",             TokenType::Increment},
        {"-",              TokenType::Minus},
        {"-=",             TokenType::SubtractAssign},
        {"--",             TokenType::Decrement},
        {"~",              TokenType::BitwiseNot},
        {"*",              TokenType::Asterisk},
        {"*=",             TokenType::MultiplyAssign},
        {"/",              TokenType::Divide},
        {"/=",             TokenType::DivideAssign},
        {"\\",             TokenType::Backslash},
        {";",              TokenType::Semicolon},
        {"{",              TokenType::LeftBrace},
        {"}",              TokenType::RightBrace},
        {"[",              TokenType::LeftBracket},
        {"]",              TokenType::RightBracket},
        {"#",              TokenType::Hash},
        {"??=",            TokenType::Hash},
        {"??(",            TokenType::LeftBracket},
        {"??/",            TokenType::Backslash},
        {"??)",            TokenType::RightBracket},
        {"??'",            TokenType::RightBracket},
        {"??!",            TokenType::BitwiseOr},
        {"??>",            TokenType::RightBrace},
        {"??<",            TokenType::LeftBrace},
        {"??-",            TokenType::BitwiseNot},
        {"(",              TokenType::LeftParenthesis},
        {")",              TokenType::RightParenthesis},
        {"^",              TokenType::ExclusiveOr},
        {"^=",             TokenType::ExclusiveOrAssign},
        {".",              TokenType::Dot},
        {"->",             TokenType::Arrow},
        {",",              TokenType::Comma},
        {"<",              TokenType::LessThan},
        {"<=",             TokenType::LessThanOrEqual},
        {"<<",             TokenType::ShiftLeft},
        {"<<=",            TokenType::ShiftLeftAssign},
        {">>",             TokenType::ShiftRight},
        {">>=",            TokenType::ShiftRightAssign},
        {">",              TokenType::GreaterThan},
        {">=",             TokenType::GreaterThanOrEqual},
        {"|",              TokenType::BitwiseOr},
        {"|=",             TokenType::BitwiseOrAssign},
        {"||",             TokenType::LogicalOr},
        {"&",              TokenType::Ampersand},
        {"&=",             TokenType::BitwiseAndAssign},
        {"&&",             TokenType::LogicalAnd},
        {"!",              TokenType::LogicalNot},
        {"!=",             TokenType::NotEquals},
        {"=",              TokenType::Assign},
        {"==",             TokenType::Equals},
        {"?",              TokenType::QuestionMark},
        {":",              TokenType::Colon},
        {"%",              TokenType::Modulo},
        {"%=",             TokenType::ModuloAssign},

        // Keywords
        {"auto",           TokenType::KeywordAuto},
        {"break",          TokenType::KeywordBreak},
        {"case",           TokenType::KeywordCase},
        {"char",           TokenType::KeywordChar},
        {"const",          TokenType::KeywordConst},
        {"continue",       TokenType::KeywordContinue},
        {"default",        TokenType::KeywordDefault},
        {"do",             TokenType::KeywordDo},
        {"double",         TokenType::KeywordDouble},
        {"else",           TokenType::KeywordElse},
        {"enum",           TokenType::KeywordEnum},
        {"extern",         TokenType::KeywordExtern},
        {"float",          TokenType::KeywordFloat},
        {"for",            TokenType::KeywordFor},
        {"goto",           TokenType::KeywordGoto},
        {"if",             TokenType::KeywordIf},
        {"inline",         TokenType::KeywordInline},
        {"int",            TokenType::KeywordInt},
        {"long",           TokenType::KeywordLong},
        {"register",       TokenType::KeywordRegister},
        {"restrict",       TokenType::KeywordRestrict},
        {"return",         TokenType::KeywordReturn},
        {"short",          TokenType::KeywordShort},
        {"signed",         TokenType::KeywordSigned},
        {"sizeof",         TokenType::KeywordSizeof},
        {"static",         TokenType::KeywordStatic},
        {"struct",         TokenType::KeywordStruct},
        {"switch",         TokenType::KeywordSwitch},
        {"typedef",        TokenType::KeywordTypedef},
        {"union",          TokenType::KeywordUnion},
        {"unsigned",       TokenType::KeywordUnsigned},
        {"void",           TokenType::KeywordVoid},
        {"volatile",       TokenType::KeywordVolatile},
        {"while",          TokenType::KeywordWhile},
        {"_Alignas",       TokenType::KeywordAlignas},
        {"_Alignof",       TokenType::KeywordAlignof},
        {"_Atomic",        TokenType::KeywordAtomic},
        {"_Bool",          TokenType::KeywordBool},
        {"_Complex",       TokenType::KeywordComplex},
        {"_Generic",       TokenType::KeywordGeneric},
        {"_Imaginary",     TokenType::KeywordImaginary},
        {"_Noreturn",      TokenType::KeywordNoreturn},
        {"_Static_assert", TokenType::KeywordStaticAssert},
        {"_Thread_local",  TokenType::KeywordThreadLocal},
};

enum class IntegerLiteralType {
    Int,
    Long,
    LongLong,
};

struct IntegerLiteralTokenValue {
    IntegerTokenValue value;
    bool isUnsigned;
    IntegerLiteralType type;
};

struct FloatingPointLiteralTokenValue {
    FloatingTokenValue value;
    bool isDouble;
};

typedef std::variant<IntegerLiteralTokenValue, FloatingPointLiteralTokenValue, std::string> TokenValue;

struct Token {
    Token(TokenType type, Span &&span, TokenValue &&value) :
            type{type}, _span{span}, value{std::move(value)} {}

    Token(TokenType type, Span &&span) :
            type{type}, _span{span}, value{} {}

    TokenType type;

    Span _span;

    TokenValue value;
};

typedef std::unique_ptr<Token> TokenPtr;
typedef std::vector<TokenPtr> TokenList;

enum class BaseConvertResult {
    Success,
    DigitNotInBase,
    InvalidDigit,
    UnexpectedEndOfInput,
};

enum class IntLiteralSuffix {
    None,
    Unsigned,
    Long,
    LongLong,
};

BaseConvertResult ConvertToBase(int base, char digit, int &digitOut);

class Tokenizer final {
public:
    Tokenizer(const std::string &filePath, std::istream &inputStream)
        : filePath{filePath}, inputStream{inputStream} {};

    void Tokenize(TokenList &tokensOut);
private:
    size_t TokenizeNormalToken();

    void TokenizeIdentifier(TokenList &tokensOut);

    void TokenizeString(TokenList &tokensOut);

    BaseConvertResult GetDigit(int base, int &digitOut);

    IntLiteralSuffix
    GetIntLiteralSuffix(IntLiteralSuffix previousSuffix, bool &isSigned_Out, IntegerLiteralType &typeOut);

    void TokenizeInteger(TokenList &tokensOut);

    bool TokenizeConstant(TokenList &tokensOut);

    bool GetNextChar();

    bool PeekNextChar(char &out);

    bool ConsumeIfNextChar(char c, bool toLower = true);

    [[nodiscard]]
    size_t GetCharIndex() const;

    [[nodiscard]]
    size_t GetColumnIndex() const;

    [[nodiscard]]
    Span SpanFromCurrent(size_t length = 1) const;

    std::string filePath;
    std::istream &inputStream;
    char currentChar{};
    size_t line{1};
    size_t lineStartIndex{};
    std::optional<TokenType> currentToken{};
};


#endif //JCC_TOKENIZER_H
