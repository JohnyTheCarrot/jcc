#include "static_tokens.h"
#include "misc/trie.h"

namespace jcc::tokenizer::static_tokens {
	using TokenTrieValue = std::variant<Keyword, Directive, Punctuator, SpecialPurpose>;

	using TokenTrie = TrieNode<'!', '~', TokenTrieValue>;
	TokenTrie const m_TokenTrie{
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
	        {"false", Keyword::False},
	        {"true", Keyword::True},
	        {"nullptr", Keyword::Nullptr},
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
	        {"//", SpecialPurpose::LineComment},
	        {"/*", SpecialPurpose::BlockComment},
	};

	[[nodiscard]]
	Token::Value TrieResultToTokenValue(TokenTrieValue value) {
		if (std::holds_alternative<Keyword>(value))
			return std::get<Keyword>(value);

		if (std::holds_alternative<Directive>(value))
			return std::get<Directive>(value);

		if (std::holds_alternative<SpecialPurpose>(value))
			return std::get<SpecialPurpose>(value);

		return std::get<Punctuator>(value);
	}

	StaticTokenTokenizationResult Tokenize(CharIter &charIter) {
		Span span{
		        charIter.GetFileName(), charIter.GetCurrentSpanMarker(), charIter.GetCurrentSpanMarker(),
		        charIter.GetInput()->tellg(), charIter.GetInput()
		};
		std::string                   unrecognizedTokenBuff{};
		TokenTrie const              *currentNode{&m_TokenTrie};
		std::optional<TokenTrieValue> trieResult{};

		while (charIter != CharIter::c_UntilNewline) {
			auto [spanMarker, character, isSentinel]{*charIter};

			// TODO: this feels like a hack, but basically we don't want to add the quote to the unrecognized token buffer because we want to handle it separately
			if (character == '\'' || character == '"')
				break;

			unrecognizedTokenBuff.push_back(character);

			auto [node, leaf]{currentNode->Find(character)};

			if (leaf != std::nullopt) {
				trieResult = leaf->m_Value;
			}

			span.m_End = spanMarker;

			if (!std::isspace(character))
				currentNode = node;

			if (node == nullptr) {
				if (charIter != CharIter::end())
					++charIter;
				break;
			}

			++charIter;
		}

		if (currentNode == nullptr ||
		    !trieResult.has_value())// if we haven't reached the end of the trie or there's no trie result
			return {.valueOrString = std::move(unrecognizedTokenBuff), .endMarker = span.m_End};

		return {.valueOrString = TrieResultToTokenValue(trieResult.value()), .endMarker = span.m_End};
	}
}// namespace jcc::tokenizer::static_tokens
