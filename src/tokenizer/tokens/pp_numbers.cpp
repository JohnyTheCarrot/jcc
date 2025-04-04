#include "pp_numbers.h"

#include <cctype> // for isdigit
#include <string> // for string
#include <utility>// for move

#include "diagnostics/variants/tk_pp/pp_number_invalid.hpp"// for PpNumber...
#include "misc/Span.h"                                     // for Span
#include "mjolnir/span.hpp"                                // for Span
#include "parsing/parser.h"                                // for Compiler...
#include "tokenizer/char_iter.h"                           // for CharIter
#include "utils.h"                                         // for IsNonDigit

namespace jcc::tokenizer::pp_numbers {
    [[nodiscard]]
    bool
    GatherDigit(CharIter &charIter, mjolnir::Span &span, std::string &number) {
        // TODO: This currently allows for a single quote to be after a non-digit character, the standard doesn't allow this.
        switch (charIter->m_Char) {
            case '\'':
                ++charIter;

                // Make sure the single quote is followed by a digit
                if (charIter == CharIter::end() ||
                    !std::isxdigit(charIter->m_Char)) {
                    auto &compilerState{parsing::CompilerState::GetInstance()};
                    compilerState.EmplaceTemporarilyFatalDiagnostic<
                            diagnostics::PpNumberInvalid>(
                            charIter.GetSource(), span
                    );
                }

                number += charIter->m_Char;
                span.expand();
                return true;
            case 'e':
            case 'E':
            case 'p':
            case 'P':
                number += charIter->m_Char;
                ++charIter;
                if (charIter == CharIter::end()) {
                    auto &compilerState{parsing::CompilerState::GetInstance()};
                    compilerState.EmplaceTemporarilyFatalDiagnostic<
                            diagnostics::PpNumberInvalid>(
                            charIter.GetSource(), span
                    );
                }

                // A pp-number that has eEpP in it doesn't necessarily mean we're going doing the eEpP route
                number += charIter->m_Char;
                span.expand();
                return true;
            case '.':
                number += charIter->m_Char;
                return true;
            default:
                break;
        }

        if (!std::isdigit(charIter->m_Char) &&
            !utils::IsNonDigit(charIter->m_Char)) {
            return false;
        }

        number += charIter->m_Char;
        return true;
    }

    Token Tokenize(CharIter &charIter, std::size_t start, bool startsWithDot) {
        // In order to have reached this point, we must have already consumed a digit.
        // We therefore know that charIter must be a digit and cannot be at the end of the input.
        std::string   number{startsWithDot ? "." : ""};
        mjolnir::Span span{start, start};

        number += charIter->m_Char;
        ++charIter;
        span.expand();

        while (charIter != CharIter::end()) {
            if (auto const hasNext{GatherDigit(charIter, span, number)};
                !hasNext)
                break;

            ++charIter;
            span.expand();
        }

        return Token{
                .m_Value = PpNumber{std::move(number)},
                .m_Span  = Span{charIter.GetSource(), span}
        };
    }
}// namespace jcc::tokenizer::pp_numbers
