#include "preprocessor.h"

#include <utility>

namespace jcc::preprocessor {
    PreprocessorToken Preprocessor::GetNextPreprocessorToken() {
        while (true) {
            if (auto ppToken{GetNextFromTokenizer()};
                !ppToken.m_Token.Is(tokenizer::SpecialPurpose::NewLine)) {
                return ppToken;
            }
        }
    }

    PreprocessorIterator Preprocessor::begin() {
        return ++PreprocessorIterator{*this};
    }

    PreprocessorIterator Preprocessor::end() {
        return PreprocessorIterator::end();
    }

    Span const &Preprocessor::GetCurrentSpan() const noexcept {
        return m_CurrentSpan;
    }

    PreprocessorToken Preprocessor::GetNextFromTokenizer(bool executeCommands) {
        while (true) {
            auto ppToken{SimpleTokenRead()};

            if (auto &[token, span]{ppToken.m_Token};
                std::holds_alternative<tokenizer::SpecialPurpose>(token)) {
                switch (std::get<tokenizer::SpecialPurpose>(token)) {
                    case tokenizer::SpecialPurpose::EndOfFile:
                    case tokenizer::SpecialPurpose::Error:
                        return ppToken;
                    default:
                        break;
                }
            }

            if (!executeCommands)
                return ppToken;

            auto const valueType{ppToken.m_Token.GetValueType()};
            auto const commandMap{
                    commands::PreprocessorCommandSingleton::GetInstance()
                            .GetCommandMap()
            };

            if (auto const command{commandMap.find(valueType)};
                command != commandMap.end()) {
                if (auto result{command->second->Execute(
                            *this, std::move(ppToken.m_Token)
                    )};
                    result.has_value())
                    // If the command does not return a token, it means it was a directive.
                    return std::move(result.value());
            } else {
                return ppToken;
            }
        }
    }

    PreprocessorToken Preprocessor::SimpleTokenRead() {
        if (auto token{m_pMacroStore->GetTokenFromMacroArgumentReader()};
            token.has_value()) {
            m_CurrentSpan = token->m_Span;

            // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
            return {std::move(token.value()), false};
        }

        if (auto token{m_pMacroStore->GetTokenFromMacroStack()};
            token.has_value()) {
            m_CurrentSpan = token->m_Span;
            return {std::move(token.value()), false
            };// used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
        }

        if (m_TokenIter == m_Tokenizer.end())
            return {{tokenizer::SpecialPurpose::EndOfFile,
                     m_Tokenizer.GetLastSpan()},
                    false};

        auto token{*m_TokenIter};
        ++m_TokenIter;
        m_CurrentSpan = token.m_Span;

        return {std::move(token), false};
    }

    MacroStore &Preprocessor::GetMacroStore() const noexcept {
        return *m_pMacroStore;
    }

    void Preprocessor::EmitWarning(Diagnosis &&diagnosis) const {
        m_pDiagnoses->emplace_back(std::move(diagnosis));
    }

    void Preprocessor::SkipEmptyLines() {
        auto token{SimpleTokenRead()};

        while (token.m_Token.Is(tokenizer::SpecialPurpose::NewLine)) {
            token = SimpleTokenRead();
        }
    }

    Preprocessor::Preprocessor(
            std::string const &filename, std::istream &ifstream,
            Diagnosis::Vec &diagnoses
    )
        : m_Tokenizer{ifstream, filename}
        , m_TokenIter{m_Tokenizer.begin()}
        , m_CurrentSpan{std::make_shared<std::string>(filename), {}, {}, &ifstream}
        , m_pDiagnoses{&diagnoses} {
        ++m_TokenIter;
    }
}// namespace jcc::preprocessor
