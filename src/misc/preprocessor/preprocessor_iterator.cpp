#include "preprocessor_iterator.h"
#include "preprocessor.h"

namespace jcc::preprocessor {
	Tokenizer::Token PreprocessorIterator::GetNextToken() const {
		return (*m_pPreprocessor)();
	}

	Tokenizer::Token InternalPreprocessorIterator::GetNextToken() const {
		return m_pPreprocessor->GetNextFromTokenizer();
	}
}
