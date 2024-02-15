#ifndef JCC_PREPROCESSOR_H
#define JCC_PREPROCESSOR_H

#include "Tokenizer.h"
#include <fstream>
#include <stack>

class Preprocessor final {
	Tokenizer                                   m_MainTokenizer;
	std::vector<std::unique_ptr<std::ifstream>> m_IStreams;
	std::stack<std::unique_ptr<Tokenizer>>      m_TokenizerStack;
	Tokenizer                                  *m_CurrentTokenizer;
	Diagnosis::Vec                             &m_Diagnoses;

	[[nodiscard]]
	bool ExecuteIncludeDirective(const Tokenizer::Token &includeDirectiveToken);

public:
	Preprocessor(std::string &&fileName, std::istream &iStream, Diagnosis::Vec &diagnoses)
	    : m_MainTokenizer{fileName, iStream, diagnoses}
	    , m_IStreams{}
	    , m_TokenizerStack{}
	    , m_CurrentTokenizer{&m_MainTokenizer}
	    , m_Diagnoses{diagnoses} {
	}

	Preprocessor(const Preprocessor &)            = delete;
	Preprocessor(Preprocessor &&)                 = delete;
	Preprocessor &operator=(const Preprocessor &) = delete;
	Preprocessor &operator=(Preprocessor &&)      = delete;

	Tokenizer::Token operator()();
};


#endif//JCC_PREPROCESSOR_H
