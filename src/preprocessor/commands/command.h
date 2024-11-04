#ifndef COMMAND_H
#define COMMAND_H

#include <memory>       // for unique_ptr
#include <optional>     // for optional
#include <unordered_map>// for unordered_map
#include <variant>      // for hash
#include <vector>       // for vector

#include "preprocessor/preprocessor_token.h"// for PreprocessorToken
#include "tokenizer/token.h"                // for Token

namespace jcc::preprocessor {
    class Preprocessor;

    namespace commands {
        using CommandMap =
                std::unordered_map<tokenizer::Token::Type, class Command *>;

        class Command {
        public:
            Command(CommandMap &map, tokenizer::Token::Type tokenType);

            virtual ~Command() noexcept = default;

            [[nodiscard]]
            virtual std::optional<PreprocessorToken>
            Execute(Preprocessor &preprocessor, tokenizer::Token &&) const = 0;
        };

        class PreprocessorCommandSingleton final {
            CommandMap                            m_CommandMap{};
            std::vector<std::unique_ptr<Command>> m_Commands{};

            PreprocessorCommandSingleton();

        public:
            static PreprocessorCommandSingleton &GetInstance();

            CommandMap const &GetCommandMap() const noexcept;
        };
    }// namespace commands
}// namespace jcc::preprocessor

#endif// COMMAND_H
