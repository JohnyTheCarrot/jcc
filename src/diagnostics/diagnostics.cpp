#include "diagnostics.h"

#include <utility>// for move

#include "mjolnir/report.hpp"// for BasicReportKind

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    FatalCompilerError::FatalCompilerError() noexcept = default;

    char const *FatalCompilerError::what() const noexcept {
        return "Fatal compiler error";
    }

    DiagnosticData::DiagnosticData(
            std::shared_ptr<Source> source, std::size_t startPos,
            mjolnir::BasicReportKind reportKind
    ) noexcept
        : m_Source{std::move(source)}
        , m_StartPos{startPos}
        , m_ReportKind{std::move(reportKind)} {
    }

    DiagnosticData::~DiagnosticData() = default;

    bool DiagnosticData::IsError() const noexcept {
        return m_ReportKind == mjolnir::BasicReportKind::Error;
    }

    BinaryDiagnostic::BinaryDiagnostic(
            mjolnir::BasicReportKind reportKind, std::shared_ptr<Source> source,
            mjolnir::Span lhsSpan, mjolnir::Span rhsSpan, mjolnir::Span opSpan,
            parsing::types::ValueType lhsType, parsing::types::ValueType rhsType
    )
        : DiagnosticData{std::move(source), lhsSpan.start(), reportKind}
        , m_LhsSpan{lhsSpan}
        , m_RhsSpan{rhsSpan}
        , m_OpSpan{opSpan}
        , m_LhsType{lhsType}
        , m_RhsType{rhsType} {
    }
}// namespace jcc::diagnostics
