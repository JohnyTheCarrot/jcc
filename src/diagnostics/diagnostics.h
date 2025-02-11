#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <memory>
#include <mjolnir/report.hpp>

#include "parsing_sema/types/type.h"
#include "source.h"

namespace jcc::diagnostics {
    class DiagnosticData {
        using Class = mjolnir::ReportKind;
        Class                   m_Class;
        std::size_t             m_StartPos;
        std::shared_ptr<Source> m_Source;

    protected:
        [[nodiscard]]
        mjolnir::Report StartReport() const;

    public:
        DiagnosticData(
                std::shared_ptr<Source> source, std::size_t startPos,
                Class diagClass
        ) noexcept;

        virtual ~DiagnosticData() = default;

        virtual void Print(std::ostream &ostream) const = 0;

        [[nodiscard]]
        Class GetClass() const noexcept;

        [[nodiscard]]
        bool IsError() const noexcept;

        [[nodiscard]]
        std::size_t GetStartPos() const noexcept;

        [[nodiscard]]
        mjolnir::Source const &GetSource() const noexcept;
    };

    class BinaryDiagnostic : public DiagnosticData {
        mjolnir::Span                  m_LhsSpan;
        mjolnir::Span                  m_RhsSpan;
        mjolnir::Span                  m_OpSpan;
        parsing_sema::types::ValueType m_LhsType;
        parsing_sema::types::ValueType m_RhsType;

    public:
        BinaryDiagnostic(
                mjolnir::ReportKind   &&reportKind,
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing_sema::types::ValueType lhsType,
                parsing_sema::types::ValueType rhsType
        );

    protected:
        [[nodiscard]]
        mjolnir::Span const &GetLhsSpan() const noexcept;

        [[nodiscard]]
        mjolnir::Span const &GetRhsSpan() const noexcept;

        [[nodiscard]]
        mjolnir::Span const &GetOpSpan() const noexcept;

        [[nodiscard]]
        parsing_sema::types::ValueType const &GetLhsType() const noexcept;

        [[nodiscard]]
        parsing_sema::types::ValueType const &GetRhsType() const noexcept;
    };

    using Diagnostic = std::unique_ptr<DiagnosticData>;
}// namespace jcc::diagnostics

#endif//DIAGNOSTICS_H
