#include "command.h"

#include "define_command.h"    // for DefineCommand
#include "else_command.h"      // for ElseCommand
#include "endif_command.h"     // for EndifCommand
#include "error_command.h"     // for ErrorCommand
#include "identifier_command.h"// for IdentifierCommand
#include "ifdef_command.h"     // for IfdefCommand
#include "include_command.h"   // for IncludeCommand
#include "undef_command.h"     // for UndefCommand
#include "warning_command.h"   // for WarningCommand

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
        m_Commands.emplace_back(std::make_unique<IncludeCommand>(m_CommandMap));
        m_Commands.emplace_back(std::make_unique<IfdefCommand>(m_CommandMap));
        m_Commands.emplace_back(
                std::make_unique<IfdefCommand>(m_CommandMap, true)
        );
        m_Commands.emplace_back(std::make_unique<EndifCommand>(m_CommandMap));
        m_Commands.emplace_back(std::make_unique<ElseCommand>(m_CommandMap));
        m_Commands.emplace_back(std::make_unique<UndefCommand>(m_CommandMap));
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
