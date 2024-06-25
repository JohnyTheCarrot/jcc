#include "tokenizer/tokenizer.h"

#include "misc/Diagnosis.h"

namespace jcc::tokenizer {
	Tokenizer::TokenTrie Tokenizer::m_TokenTrie{
	        {"auto", Keyword::Auto},
	        {"break", Keyword::Break},
	        {"case", Keyword::Case},
	        {"char", Keyword::Char},
	        {"const", Keyword::Const},
	        {"continue", Keyword::Continue},
	        {"default", Keyword::Default},
	        {"do", Keyword::Do},
	        {"double", Keyword::Double},
	        {"else", Keyword::Else},
	        {"enum", Keyword::Enum},
	        {"extern", Keyword::Extern},
	        {"float", Keyword::Float},
	        {"for", Keyword::For},
	        {"goto", Keyword::Goto},
	        {"if", Keyword::If},
	        {"inline", Keyword::Inline},
	        {"int", Keyword::Int},
	        {"long", Keyword::Long},
	        {"register", Keyword::Register},
	        {"restrict", Keyword::Restrict},
	        {"return", Keyword::Return},
	        {"short", Keyword::Short},
	        {"signed", Keyword::Signed},
	        {"sizeof", Keyword::Sizeof},
	        {"static", Keyword::Static},
	        {"struct", Keyword::Struct},
	        {"switch", Keyword::Switch},
	        {"typedef", Keyword::Typedef},
	        {"union", Keyword::Union},
	        {"unsigned", Keyword::Unsigned},
	        {"void", Keyword::Void},
	        {"volatile", Keyword::Volatile},
	        {"while", Keyword::While},
	        {"_Alignas", Keyword::Alignas},
	        {"_Alignof", Keyword::Alignof},
	        {"_Atomic", Keyword::Atomic},
	        {"_Bool", Keyword::Bool},
	        {"_Complex", Keyword::Complex},
	        {"_Generic", Keyword::Generic},
	        {"_Imaginary", Keyword::Imaginary},
	        {"_Noreturn", Keyword::Noreturn},
	        {"_Static_assert", Keyword::StaticAssert},
	        {"_Thread_local", Keyword::ThreadLocal},
	        {"include", Directive::Include},
	        {"define", Directive::Define},
	        {"undef", Directive::Undef},
	        {"line", Directive::Line},
	        {"error", Directive::Error},
	        {"pragma", Directive::Pragma},
	        {"if", Directive::If},
	        {"ifdef", Directive::Ifdef},
	        {"ifndef", Directive::Ifndef},
	        {"elif", Directive::Elif},
	        {"else", Directive::Else},
	        {"endif", Directive::Endif},
	        {"[", Punctuator::LeftBracket},
	        {"]", Punctuator::RightBracket},
	        {"(", Punctuator::PpLeftParenthesis},
	        {")", Punctuator::RightParenthesis},
	        {"{", Punctuator::LeftBrace},
	        {"}", Punctuator::RightBrace},
	        {".", Punctuator::Dot},
	        {"...", Punctuator::Ellipsis},
	        {"-", Punctuator::Minus},
	        {"--", Punctuator::MinusMinus},
	        {"-=", Punctuator::MinusEqual},
	        {"->", Punctuator::Arrow},
	        {"+", Punctuator::Plus},
	        {"++", Punctuator::PlusPlus},
	        {"+=", Punctuator::PlusEqual},
	        {"&", Punctuator::Ampersand},
	        {"&&", Punctuator::AmpersandAmpersand},
	        {"&=", Punctuator::AmpersandEqual},
	        {"|", Punctuator::VerticalBar},
	        {"||", Punctuator::VerticalBarVerticalBar},
	        {"|=", Punctuator::VerticalBarEqual},
	        {"^", Punctuator::Caret},
	        {"^=", Punctuator::CaretEqual},
	        {"<", Punctuator::LessThan},
	        {"<<", Punctuator::LessThanLessThan},
	        {"<=", Punctuator::LessThanEqual},
	        {"<<=", Punctuator::LessThanLessThanEqual},
	        {">", Punctuator::GreaterThan},
	        {">>", Punctuator::GreaterThanGreaterThan},
	        {">=", Punctuator::GreaterThanEqual},
	        {">>=", Punctuator::GreaterThanGreaterThanEqual},
	        {"=", Punctuator::Equal},
	        {"==", Punctuator::EqualEqual},
	        {",", Punctuator::Comma},
	        {":", Punctuator::Colon},
	        {";", Punctuator::Semicolon},
	        {"#", Punctuator::Hash},
	        {"##", Punctuator::HashHash},
	        {"?", Punctuator::QuestionMark},
	        {"*", Punctuator::Asterisk},
	        {"*=", Punctuator::AsteriskEqual},
	        {"/", Punctuator::Slash},
	        {"/=", Punctuator::SlashEqual},
	        {"%", Punctuator::Percent},
	        {"%=", Punctuator::PercentEqual},
	        {"~", Punctuator::Tilde},
	        {"<:", Punctuator::LeftBracket},
	        {":>", Punctuator::RightBracket},
	        {"<%", Punctuator::LeftBrace},
	        {"%>", Punctuator::RightBrace},
	        {"%:", Punctuator::Hash},
	        {"%:%:", Punctuator::HashHash},
	        {"!", Punctuator::ExclamationMark},
	        {"!=", Punctuator::ExclamationMarkEqual},
	};

	std::optional<SpanMarker> Tokenizer::CollectRestOfIdentifier(std::string &identifier) {
		std::optional<SpanMarker> lastSpanMarker{};

		for (CharIter const untilNewline{'\n'}; m_CharIter != untilNewline; ++m_CharIter) {
			auto const [spanMarker, character, isSentinel]{*m_CharIter};

			if (!Identifier::IsValidChar(character))
				break;

			identifier.push_back(character);
			lastSpanMarker = spanMarker;
		}

		return lastSpanMarker;
	}

	Tokenizer::TrieTraversalResult Tokenizer::TraverseTrie() {
		Span           span{m_FileName, m_CharIter->m_SpanMarker, m_CharIter->m_SpanMarker, m_Input->tellg(), m_Input};
		CharIter const untilNewline{'\n'};
		std::string    identifier{};
		TokenTrie const              *currentNode{&m_TokenTrie};
		std::optional<TokenTrieValue> trieResult{};

		for (; m_CharIter != untilNewline; ++m_CharIter) {
			auto [spanMarker, character, isSentinel]{*m_CharIter};
			identifier.push_back(character);

			auto [node, leaf]{currentNode->Find(character)};

			if (leaf != std::nullopt) {
				trieResult = leaf->m_Value;
			}

			span.m_End = spanMarker;

			currentNode = node;

			if (node == nullptr) {
				if (m_CharIter != CharIter::end())
					++m_CharIter;
				break;
			}
		}

		if (currentNode == nullptr ||
		    !trieResult.has_value())// if we haven't reached the end of the trie or there's no trie result
			return {.valueOrString = std::move(identifier), .endMarker = span.m_End};

		return {.valueOrString = TrieResultToTokenValue(trieResult.value()), .endMarker = span.m_End};
	}

	Token Tokenizer::CollectIdentifier(
	        std::string_view partialIdentifier, SpanMarker identifierStart, std::streampos const &identifierStartPos
	) {
		std::string identifier{};
		identifier.reserve(partialIdentifier.length());
		std::ranges::move(partialIdentifier, std::back_inserter(identifier));
		SpanMarker endMarker{};

		if (auto const optSpanMarker{CollectRestOfIdentifier(identifier)}; optSpanMarker.has_value()) {
			endMarker = optSpanMarker.value();
		}

		Span span{m_FileName, identifierStart, endMarker, identifierStartPos, m_Input};
		// check if identifier contains a backslash
		if (identifier.find('\\') != std::string::npos)
			// TODO: Universal character names
			throw FatalCompilerError{Diagnosis::Kind::TODO, std::move(span)};

		Identifier identToken{std::move(identifier)};

		return Token{.m_Value = std::move(identToken), .m_Span = std::move(span)};
	}

	bool Tokenizer::SkipWhitespace() {
		CharIter const untilNewline{'\n'};
		bool           skippedWhitespace{false};

		while (m_CharIter != untilNewline && std::isspace(m_CharIter->m_Char)) {
			++m_CharIter;
			skippedWhitespace = true;
		}

		return skippedWhitespace;
	}

	Token::Value Tokenizer::TrieResultToTokenValue(TokenTrieValue value) {
		if (std::holds_alternative<Keyword>(value))
			return std::get<Keyword>(value);

		if (std::holds_alternative<Directive>(value))
			return std::get<Directive>(value);

		return std::get<Punctuator>(value);
	}

	Tokenizer::Tokenizer(std::istream &input, std::string_view fileName)
	    : m_Input{&input}
	    , m_CharIter{input, fileName} {
	}

	std::optional<Token> Tokenizer::GetNextToken() {
		if (m_CharIter == CharIter::end())
			return std::nullopt;

		bool const skippedWhitespace{SkipWhitespace()};

		if (m_CharIter == CharIter::end())
			return std::nullopt;

		Span span{m_FileName, m_CharIter->m_SpanMarker, m_CharIter->m_SpanMarker, m_Input->tellg(), m_Input};
		if (m_CharIter->m_Char == '\n') {
			return Token{SpecialPurpose::NewLine, std::move(span)};
		}

		bool const couldBeIdentifier{Identifier::IsValidFirstChar(m_CharIter->m_Char)};
		auto const [valueOrString, trieResultEndMarker]{TraverseTrie()};
		span.m_End = trieResultEndMarker;

		if (couldBeIdentifier && std::holds_alternative<std::string>(valueOrString)) {
			auto partialIdentifier{std::get<std::string>(valueOrString)};
			return CollectIdentifier(std::move(partialIdentifier), span.m_Start, span.m_StartPos);
		}

		auto const tokenValue{[&]() -> Token::Value {
			if (!std::holds_alternative<Token::Value>(valueOrString))
				throw FatalCompilerError{Diagnosis::Kind::TK_UnexpectedChar, std::move(span)};

			auto tokenValue{std::get<Token::Value>(valueOrString)};
			if (std::holds_alternative<Punctuator>(tokenValue)) {
				if (auto const punctuator{std::get<Punctuator>(tokenValue)};
				    punctuator == Punctuator::PpLeftParenthesis && skippedWhitespace)
					tokenValue = Punctuator::LeftParenthesis;
			}

			return tokenValue;
		}()};

		return Token{.m_Value = tokenValue, .m_Span = std::move(span)};
	}

	TokenizerIterator Tokenizer::begin() {
		return TokenizerIterator{*this};
	}

	TokenizerIterator Tokenizer::end() {
		return TokenizerIterator{};
	}
}// namespace jcc::tokenizer
