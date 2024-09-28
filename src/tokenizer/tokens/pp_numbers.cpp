#include "pp_numbers.h"

#include <string>
#include <utility>

#include "misc/Diagnosis.h"
#include "tokenizer/char_iter.h"
#include "utils.h"

namespace jcc::tokenizer::pp_numbers {
    [[nodiscard]]
    bool GatherDigit(CharIter &charIter, Span &span, std::string &number) {
        switch (charIter->m_Char) {
            case '\'':
                ++charIter;

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
        std::string number{startsWithDot ? "." : ""};
        Span        span{
                charIter.GetFileName(), start, start,
                charIter.GetInput()
        };

        if (charIter == CharIter::end()) {
            return Token{
                    .m_Value = PpNumber{std::move(number)},
                    .m_Span  = std::move(span)
            };
        }

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
