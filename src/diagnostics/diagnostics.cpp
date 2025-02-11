#include "diagnostics.h"

#include <utility>

namespace jcc::diagnostics {
    mjolnir::Report DiagnosticData::StartReport() const {
        return {m_Class, m_Source->m_Source, m_StartPos};
    }

    DiagnosticData::DiagnosticData(
            std::shared_ptr<Source> source, std::size_t startPos,
            Class diagClass
    ) noexcept
        : m_Class{std::move(diagClass)}
        , m_StartPos{startPos}
        , m_Source{std::move(source)} {
    }

    DiagnosticData::Class DiagnosticData::GetClass() const noexcept {
        return m_Class;
    }

    bool DiagnosticData::IsError() const noexcept {
        if (std::holds_alternative<mjolnir::BasicReportKind>(m_Class)) {
            return std::get<mjolnir::BasicReportKind>(m_Class) ==
                   mjolnir::BasicReportKind::Error;
        }

        return false;
    }

    std::size_t DiagnosticData::GetStartPos() const noexcept {
        return m_StartPos;
    }

    mjolnir::Source const &DiagnosticData::GetSource() const noexcept {
        return m_Source->m_Source;
    }

    BinaryDiagnostic::BinaryDiagnostic(
            mjolnir::ReportKind &&kind, std::shared_ptr<Source> source,
            mjolnir::Span lhsSpan, mjolnir::Span rhsSpan, mjolnir::Span opSpan,
            parsing_sema::types::ValueType lhsType,
            parsing_sema::types::ValueType rhsType
    )
        : DiagnosticData{std::move(source), lhsSpan.start(), std::move(kind)}
        , m_LhsSpan{lhsSpan}
        , m_RhsSpan{rhsSpan}
        , m_OpSpan{opSpan}
        , m_LhsType{lhsType}
        , m_RhsType{rhsType} {
    }

    mjolnir::Span const &BinaryDiagnostic::GetLhsSpan() const noexcept {
        return m_LhsSpan;
    }

    mjolnir::Span const &BinaryDiagnostic::GetRhsSpan() const noexcept {
        return m_RhsSpan;
    }

    mjolnir::Span const &BinaryDiagnostic::GetOpSpan() const noexcept {
        return m_OpSpan;
    }

    parsing_sema::types::ValueType const &
    BinaryDiagnostic::GetLhsType() const noexcept {
        return m_LhsType;
    }

    parsing_sema::types::ValueType const &
    BinaryDiagnostic::GetRhsType() const noexcept {
        return m_RhsType;
    }
}// namespace jcc::diagnostics
