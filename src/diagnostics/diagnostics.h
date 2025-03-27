#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <memory>
#include <mjolnir/report.hpp>

#include "parsing_sema/types/type.h"
#include "source.h"

namespace jcc::diagnostics {
    class DiagnosticsVisitor;

    class FatalCompilerError final : public std::exception {
    public:
        FatalCompilerError() noexcept;

        [[nodiscard]]
        char const *what() const noexcept override;
    };

    struct DiagnosticData {
        std::shared_ptr<Source>  m_Source;
        std::size_t              m_StartPos;
        mjolnir::BasicReportKind m_ReportKind;

        DiagnosticData(
                std::shared_ptr<Source> source, std::size_t startPos,
                mjolnir::BasicReportKind reportKind
        ) noexcept;

        virtual ~DiagnosticData();

        [[nodiscard]]
        bool IsError() const noexcept;

        virtual void Visit(DiagnosticsVisitor const &visitor) const = 0;
    };

    struct BinaryDiagnostic : DiagnosticData {
        mjolnir::Span                  m_LhsSpan;
        mjolnir::Span                  m_RhsSpan;
        mjolnir::Span                  m_OpSpan;
        parsing_sema::types::ValueType m_LhsType;
        parsing_sema::types::ValueType m_RhsType;

        BinaryDiagnostic(
                mjolnir::BasicReportKind reportKind,
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing_sema::types::ValueType lhsType,
                parsing_sema::types::ValueType rhsType
        );
    };

    using Diagnostic = std::unique_ptr<DiagnosticData>;
}// namespace jcc::diagnostics

#endif//DIAGNOSTICS_H
