#include "command.h"

#include "define_command.h"
#include "diagnostics/variants/todo.hpp"
#include "else_command.h"
#include "endif_command.h"
#include "error_command.h"
#include "identifier_command.h"
#include "ifdef_command.h"
#include "include_command.h"
#include "parsing/parser.h"
#include "preprocessor/preprocessor.h"
#include "undef_command.h"
#include "warning_command.h"

namespace jcc::preprocessor::commands {
    Command::Command(CommandMap &map, tokenizer::Token::Type tokenType) {
        map.emplace(tokenType, this);
    }

    PreprocessorCommandSingleton::PreprocessorCommandSingleton() {
        m_Commands.emplace_back(std::make_unique<DefineCommand>(m_CommandMap));
        m_Commands.emplace_back(
                std::make_unique<IdentifierCommand>(m_CommandMap)
        );
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
