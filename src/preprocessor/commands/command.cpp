#include "command.h"

#include "define_command.h"
#include "error_command.h"
#include "identifier_command.h"
#include "warning_command.h"

namespace jcc::preprocessor::commands {
    Command::Command(CommandMap &map, tokenizer::Token::Type tokenType) {
        map.emplace(tokenType, this);
    }

    PreprocessorCommandSingleton::PreprocessorCommandSingleton() {
        m_Commands.emplace_back(std::make_unique<DefineCommand>(m_CommandMap));
        m_Commands.emplace_back(std::make_unique<IdentifierCommand>(m_CommandMap
        ));
        m_Commands.emplace_back(std::make_unique<WarningCommand>(m_CommandMap));
        m_Commands.emplace_back(std::make_unique<ErrorCommand>(m_CommandMap));
    }

    CommandMap const &
    PreprocessorCommandSingleton::GetCommandMap() const noexcept {
        return m_CommandMap;
    }

    PreprocessorCommandSingleton &PreprocessorCommandSingleton::GetInstance() {
        static PreprocessorCommandSingleton instance{};
        return instance;
    }
}// namespace jcc::preprocessor::commands
