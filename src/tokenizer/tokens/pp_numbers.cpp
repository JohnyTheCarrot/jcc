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
    bool
    GatherDigit(CharIter &charIter, std::size_t &end, std::string &number) {
        // TODO: This currently allows for a single quote to be after a non-digit character, the standard doesn't allow this.
        switch (charIter->m_Char) {
            case '\'':
                ++charIter;

                // Make sure the single quote is followed by a digit
                if (charIter == CharIter::end() ||
                    !std::isxdigit(charIter->m_Char))
                    // TODO: Diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::TK_PreprocessingNumberInvalid,
                            // std::move(span)
                    };

                number += charIter->m_Char;
                ++end;
                return true;
            case 'e':
            case 'E':
            case 'p':
            case 'P':
                number += charIter->m_Char;
                ++charIter;
                if (charIter == CharIter::end())
                    // TODO: Diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::TK_PreprocessingNumberInvalid,
                            // std::move(span)
                    };

                // A pp-number that has eEpP in it doesn't necessarily mean we're going doing the eEpP route
                number += charIter->m_Char;
                ++end;
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
        std::string number{startsWithDot ? "." : ""};
        std::size_t end{start};

        number += charIter->m_Char;
        ++charIter;
        ++end;

        while (charIter != CharIter::end()) {
            if (auto const hasNext{GatherDigit(charIter, end, number)};
                !hasNext)
                break;

            ++charIter;
            ++end;
        }

        return Token{
                .m_Value = PpNumber{std::move(number)},
                .m_Span  = Span{charIter.GetSource(), {start, end}}
        };
    }
}// namespace jcc::tokenizer::pp_numbers
