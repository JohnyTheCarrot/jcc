#include "pp_numbers.h"

#include <cctype> // for isdigit
#include <string> // for allocator, string
#include <utility>// for move

#include "misc/Diagnosis.h"     // for Diagnosis, FatalCompilerError
#include "misc/Span.h"          // for Span, SpanMarker (ptr only)
#include "tokenizer/char_iter.h"// for CharIter
#include "utils.h"              // for IsNonDigit

namespace jcc::tokenizer::pp_numbers {
    [[nodiscard]]
    bool GatherDigit(CharIter &charIter, Span &span, std::string &number) {
        switch (charIter->m_Char) {
            case '\'':
                ++charIter;

                // Make sure the single quote is followed by a digit
                if (charIter == CharIter::end() ||
                    (!utils::IsNonDigit(charIter->m_Char) &&
                     std::isdigit(charIter->m_Char)))
                    throw FatalCompilerError{
                            Diagnosis::Kind::TK_PreprocessingNumberInvalid,
                            std::move(span)
                    };

                number += charIter->m_Char;
                return true;
            case 'e':
            case 'E':
            case 'p':
            case 'P':
                number += charIter->m_Char;
                ++charIter;
                // A pp-number that has eEpP in it doesn't necessarily mean we're going doing the eEpP route
                number += charIter->m_Char;
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

    Token
    Tokenize(CharIter &charIter, SpanMarker const &start, bool startsWithDot) {
        // In order to have reached this point, we must have already consumed a digit.
        // We therefore know that charIter must be a digit and cannot be at the end of the input.
        std::string number{startsWithDot ? "." : ""};
        Span span{charIter.GetFileName(), start, start, charIter.GetInput()};

        number += charIter->m_Char;
        ++charIter;

        while (charIter != CharIter::end()) {
            if (auto const hasNext{GatherDigit(charIter, span, number)};
                !hasNext)
                break;

            ++charIter;
        }

        return Token{
                .m_Value = PpNumber{std::move(number)},
                .m_Span  = std::move(span)
        };
    }
}// namespace jcc::tokenizer::pp_numbers
