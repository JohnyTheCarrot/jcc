#include "type.h"

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

#include "parsing_sema/parser.h"
#include "parsing_sema/types.h"

namespace jcc::parsing_sema::types {
    BitInteger::BitInteger(int bitWidth)
        : m_BitWidth{bitWidth} {
    }

    int BitInteger::GetBitWidth() const noexcept {
        return m_BitWidth;
    }

    llvm::Type *BitInteger::GetLLVMType() const {
        auto &context{CompilerState::GetInstance().GetContext()};

        return llvm::Type::getIntNTy(context, m_BitWidth);
    }

    int_ranks::IntegerConversionRank BitInteger::GetConversionRank() const {
        return int_ranks::c_LongLongRank + m_BitWidth;
    }

    void PrintTo(BitInteger const &bitInteger, std::ostream *os) {
        *os << "_BitInt(" << bitInteger.GetBitWidth() << ')';
    }

    void PrintTo(StandardIntegerType const &bitInteger, std::ostream *os) {
        switch (bitInteger) {
            case StandardIntegerType::Bool:
                *os << "bool";
                break;
            case StandardIntegerType::Char:
                *os << "char";
                break;
            case StandardIntegerType::Short:
                *os << "short";
                break;
            case StandardIntegerType::Int:
                *os << "int";
                break;
            case StandardIntegerType::Long:
                *os << "long";
                break;
            case StandardIntegerType::LongLong:
                *os << "long long";
                break;
        }
    }

    IntegerType::IntegerType(Type type, Signedness sign)
        : m_Type{std::move(type)}
        , m_Sign{sign} {
    }

    IntegerType::Type IntegerType::GetType() const noexcept {
        return m_Type;
    }

    IntegerType::Signedness IntegerType::GetSignedness() const noexcept {
        return m_Sign;
    }

    bool IntegerType::operator==(IntegerType const &other) const {
        if (std::holds_alternative<StandardIntegerType>(m_Type)) {
            if (!std::holds_alternative<StandardIntegerType>(other.m_Type))
                return false;

            auto const typeLhs{std::get<StandardIntegerType>(m_Type)};
            auto const typeRhs{std::get<StandardIntegerType>(other.m_Type)};

            // char != signed char != unsigned char
            if (typeLhs == StandardIntegerType::Char ||
                typeRhs == StandardIntegerType::Char) {
                return typeLhs == typeRhs && m_Sign == other.m_Sign;
            }
        }

        auto const signednessLhs{
                m_Sign != Signedness::Unsigned ? Signedness::Signed
                                               : Signedness::Unsigned
        };
        auto const signednessRhs{
                other.m_Sign != Signedness::Unsigned ? Signedness::Signed
                                                     : Signedness::Unsigned
        };

        return m_Type == other.m_Type && signednessLhs == signednessRhs;
    }

    bool IntegerType::IsSigned() const noexcept {
        return m_Sign == Signedness::Signed;
    }

    llvm::Type *IntegerType::GetLLVMType(Type type) {
        auto &context{CompilerState::GetInstance().GetContext()};

        if (std::holds_alternative<StandardIntegerType>(type)) {
            switch (std::get<StandardIntegerType>(type)) {
                case StandardIntegerType::Bool:
                    return llvm::Type::getInt1Ty(context);
                case StandardIntegerType::Char:
                    return llvm::Type::getInt8Ty(context);
                case StandardIntegerType::Short:
                    return llvm::Type::getInt16Ty(context);
                case StandardIntegerType::Int:
                    return llvm::Type::getInt32Ty(context);
                case StandardIntegerType::Long:
                case StandardIntegerType::LongLong:
                    return llvm::Type::getInt64Ty(context);
                default:
                    assert(false);
                    return nullptr;
            }
        }

        if (std::holds_alternative<BitInteger>(type)) {
            return std::get<BitInteger>(type).GetLLVMType();
        }

        assert(false);
        return nullptr;
    }

    llvm::Type *IntegerType::GetLLVMType() const {
        return GetLLVMType(m_Type);
    }

    unsigned IntegerType::GetBitWidth() const {
        auto const llvmType{GetLLVMType()};

        return llvmType->getIntegerBitWidth();
    }

    int_ranks::IntegerConversionRank IntegerType::GetConversionRank() const {
        if (std::holds_alternative<StandardIntegerType>(m_Type)) {
            return static_cast<int>(std::get<StandardIntegerType>(m_Type));
        }

        return std::get<BitInteger>(m_Type).GetConversionRank();
    }

    IntegerType IntegerType::Promote() const {
        auto const llvmType{GetLLVMType()};
        auto const llvmInt{GetLLVMType(StandardIntegerType::Int)};
        auto const numBits{
                llvmType->getIntegerBitWidth() /* sizeof(T) */ - IsSigned()
        };
        auto const numUnsignedIntBits{llvmInt->getIntegerBitWidth()};

        // Check if the type is promotable to signed int
        if (numBits < numUnsignedIntBits - 1 /* sign bit */) {
            return IntegerType{
                    Type{StandardIntegerType::Int}, Signedness::Signed
            };
        }

        // Check if the type is promotable to unsigned int
        if (numBits < numUnsignedIntBits) {
            return IntegerType{
                    Type{StandardIntegerType::Int}, Signedness::Unsigned
            };
        }

        // Type is not promotable to int, leave it as is
        return *this;
    }

    bool IntegerType::CanRepresentValuesOf(IntegerType const &other) const {
        auto const &lhsType{GetLLVMType()};
        auto const  lhsSigned{IsSigned()};
        auto const &rhsType{other.GetLLVMType()};
        auto const  rhsSigned{other.IsSigned()};

        return lhsType->getIntegerBitWidth() - lhsSigned >=
               rhsType->getIntegerBitWidth() - rhsSigned;
    }

    IntegerType
    IntegerType::UsualArithmeticConversion(IntegerType other) const {
        // Promote both operands
        auto const promotedLhs{Promote()};
        auto const promotedRhs{other.Promote()};

        // If both operands have the same type, then no further conversion is needed
        if (promotedLhs == promotedRhs)
            return promotedLhs;

        // If both operands have the same signedness, then the operand with the lower rank is converted to the type of the operand with the higher rank
        if (promotedLhs.IsSigned() == promotedRhs.IsSigned())
            return promotedLhs.GetConversionRank() >
                                   promotedRhs.GetConversionRank()
                         ? promotedLhs
                         : promotedRhs;

        auto const &unsignedType{
                promotedLhs.IsSigned() ? promotedRhs : promotedLhs
        };
        auto const &signedType{
                promotedLhs.IsSigned() ? promotedLhs : promotedRhs
        };

        // If the unsigned type has a conversion rank greater than or equal to the signed type, then the signed type is converted to the unsigned type
        if (unsignedType.GetConversionRank() >= signedType.GetConversionRank())
            return unsignedType;

        // If the signed type can represent all values of the unsigned type, then the unsigned type is converted to the signed type
        if (signedType.CanRepresentValuesOf(unsignedType))
            return signedType;

        // Otherwise, both operands are converted to the unsigned type corresponding to the signed type
        return IntegerType{signedType.m_Type, Signedness::Unsigned};
    }

    FloatingType
    IntegerType::UsualArithmeticConversion(FloatingType other) const {
        return other;
    }

    void PrintTo(IntegerType type, std::ostream *os) {
        switch (type.GetSignedness()) {
            case IntegerType::Signedness::Signed:
                *os << "signed ";
                break;
            case IntegerType::Signedness::Unsigned:
                *os << "unsigned ";
                break;
            default:
                break;
        }

        std::visit([&os](auto const &t) { PrintTo(t, os); }, type.GetType());
    }

    void PrintTo(StandardFloatingType floatingType, std::ostream *os) {
        switch (floatingType) {
            case StandardFloatingType::Float:
                *os << "float";
                break;
            case StandardFloatingType::Double:
                *os << "double";
                break;
            case StandardFloatingType::LongDouble:
                *os << "long double";
                break;
            default:
                *os << "(unknown floating type)";
                break;
        }
    }

    FloatingType::FloatingType(Type &&specifier)
        : m_Type{std::move(specifier)} {
    }

    llvm::Type *FloatingType::GetLLVMType() const {
        auto &context{CompilerState::GetInstance().GetContext()};

        if (std::holds_alternative<StandardFloatingType>(m_Type)) {
            switch (std::get<StandardFloatingType>(m_Type)) {
                case StandardFloatingType::Float:
                    return llvm::Type::getFloatTy(context);
                case StandardFloatingType::Double:
                    return llvm::Type::getDoubleTy(context);
                case StandardFloatingType::LongDouble:
                    return llvm::Type::getX86_FP80Ty(context);
            }

            assert(false);
            return nullptr;
        }

        throw std::runtime_error{
                "TODO: Implement GetLLVMType for non-standard floating types"
        };
    }

    bool FloatingType::operator==(FloatingType const &other) const {
        return m_Type == other.m_Type;
    }

    FloatingType::Type FloatingType::GetType() const noexcept {
        return m_Type;
    }

    FloatingType
    FloatingType::UsualArithmeticConversion(FloatingType const &other) const {
        if (std::holds_alternative<StandardFloatingType>(m_Type) &&
            std::holds_alternative<StandardFloatingType>(other.m_Type)) {
            auto const lhs{std::get<StandardFloatingType>(m_Type)};
            auto const rhs{std::get<StandardFloatingType>(other.m_Type)};

            using Underlying = std::underlying_type_t<StandardFloatingType>;
            auto const lhsRank{static_cast<Underlying>(lhs)};
            auto const rhsRank{static_cast<Underlying>(rhs)};

            if (lhsRank >= rhsRank)
                return *this;

            return other;
        }

        // TODO: Implement UsualArithmeticConversion for non-standard floating types
        throw std::runtime_error{"TODO: Implement UsualArithmeticConversion "
                                 "for non-standard floating types"};
    }

    void PrintTo(FloatingType const &type, std::ostream *os) {
        std::visit([&os](auto const &t) { PrintTo(t, os); }, type.GetType());
    }

    ValueType::ValueType(Type &&type)
        : m_Type{type} {
    }

    [[nodiscard]]
    IntegerType::Signedness GetSignedness(BasicTypeSpecifier specifier) {
        using namespace magic_enum::bitwise_operators;

        if ((specifier & BasicTypeSpecifier::Signed) ==
            BasicTypeSpecifier::Signed)
            return IntegerType::Signedness::Signed;

        if ((specifier & BasicTypeSpecifier::Unsigned) ==
            BasicTypeSpecifier::Unsigned)
            return IntegerType::Signedness::Unsigned;

        return IntegerType::Signedness::Unspecified;
    }

    ValueType::ValueType(TypeSpecifier const &typeSpecifiers)
        : m_Type{[typeSpecifiers] -> Type {
            using namespace magic_enum::bitwise_operators;

            switch (auto const specifiers{typeSpecifiers.GetBasicSpecifiers()}
            ) {
                case BasicTypeSpecifier::Bool:
                    return IntegerType{
                            StandardIntegerType::Bool,
                            IntegerType::Signedness::Unspecified
                    };
                case BasicTypeSpecifier::Char:
                    return IntegerType{
                            StandardIntegerType::Char, GetSignedness(specifiers)
                    };
                case BasicTypeSpecifier::Short:
                case BasicTypeSpecifier::Short | BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::Short | BasicTypeSpecifier::Signed:
                case BasicTypeSpecifier::Short | BasicTypeSpecifier::Signed |
                        BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::Short | BasicTypeSpecifier::Unsigned:
                case BasicTypeSpecifier::Short | BasicTypeSpecifier::Unsigned |
                        BasicTypeSpecifier::Int:
                    return IntegerType{
                            StandardIntegerType::Short,
                            GetSignedness(specifiers)
                    };
                case BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::Signed:
                case BasicTypeSpecifier::Int | BasicTypeSpecifier::Signed:
                case BasicTypeSpecifier::Unsigned:
                case BasicTypeSpecifier::Int | BasicTypeSpecifier::Unsigned:
                    return IntegerType{
                            StandardIntegerType::Int, GetSignedness(specifiers)
                    };
                case BasicTypeSpecifier::Long:
                case BasicTypeSpecifier::Long | BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::Long | BasicTypeSpecifier::Signed:
                case BasicTypeSpecifier::Long | BasicTypeSpecifier::Signed |
                        BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::Long | BasicTypeSpecifier::Unsigned:
                case BasicTypeSpecifier::Long | BasicTypeSpecifier::Unsigned |
                        BasicTypeSpecifier::Int:
                    return IntegerType{
                            StandardIntegerType::Long, GetSignedness(specifiers)
                    };
                case BasicTypeSpecifier::LongLong:
                case BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Signed:
                case BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Signed |
                        BasicTypeSpecifier::Int:
                case BasicTypeSpecifier::LongLong |
                        BasicTypeSpecifier::Unsigned:
                case BasicTypeSpecifier::LongLong |
                        BasicTypeSpecifier::Unsigned | BasicTypeSpecifier::Int:
                    return IntegerType{
                            StandardIntegerType::LongLong,
                            GetSignedness(specifiers)
                    };
                case BasicTypeSpecifier::BitInt:
                case BasicTypeSpecifier::BitInt | BasicTypeSpecifier::Signed:
                    // TODO
                    throw diagnostics::FatalCompilerError{};
                case BasicTypeSpecifier::Float:
                    return FloatingType{StandardFloatingType::Float};
                case BasicTypeSpecifier::Double:
                    return FloatingType{StandardFloatingType::Double};
                case BasicTypeSpecifier::LongDouble:
                    return FloatingType{StandardFloatingType::LongDouble};
                // TODO: the other types
                default:
                    throw std::runtime_error{"Unhandled type"};
            }
        }()} {
    }

    bool ValueType::IsArithmetic() const noexcept {
        if (std::holds_alternative<IntegerType>(m_Type) ||
            std::holds_alternative<FloatingType>(m_Type))
            return true;

        // TODO: other arithmetic types

        return false;
    }

    bool ValueType::IsInteger() const noexcept {
        return std::holds_alternative<IntegerType>(m_Type);
    }

    bool ValueType::IsFloating() const noexcept {
        return std::holds_alternative<FloatingType>(m_Type);
    }

    llvm::Type *ValueType::GetLLVMType() const {
        return std::visit([](auto &&arg) { return arg.GetLLVMType(); }, m_Type);
    }

    ValueType::Type const &ValueType::GetType() const noexcept {
        return m_Type;
    }

    bool ValueType::operator==(ValueType const &other) const {
        return m_Type == other.m_Type;
    }

    std::string ValueType::ToString() const {
        std::ostringstream oss;

        PrintTo(*this, &oss);

        return oss.str();
    }

    llvm::Value *
    CastValue(llvm::Value *value, ValueType const &from, ValueType const &to) {
        auto &builder{CompilerState::GetInstance().GetBuilder()};

        if (from == to)
            return value;

        if (from.IsInteger() && to.IsInteger()) {
            auto const fromIntegerType{std::get<IntegerType>(from.GetType())};
            auto const toIntegerType{std::get<IntegerType>(to.GetType())};

            if (fromIntegerType.IsSigned()) {
                return builder.CreateSExt(value, toIntegerType.GetLLVMType());
            }

            return builder.CreateZExt(value, toIntegerType.GetLLVMType());
        }

        if (from.IsFloating() && to.IsFloating()) {
            auto const toFloatingType{std::get<FloatingType>(to.GetType())};

            return builder.CreateFPCast(
                    value, toFloatingType.GetLLVMType(), "fp_cast"
            );
        }

        if (from.IsInteger() && to.IsFloating()) {
            auto const integerType{std::get<IntegerType>(from.GetType())};
            auto const floatingType{std::get<FloatingType>(to.GetType())};

            if (integerType.IsSigned()) {
                return builder.CreateSIToFP(
                        value, floatingType.GetLLVMType(), "signed_int_to_float"
                );
            }

            return builder.CreateUIToFP(
                    value, floatingType.GetLLVMType(), "unsigned_int_to_float"
            );
        }

        if (from.IsFloating() && to.IsInteger()) {
            auto const integerType{std::get<IntegerType>(to.GetType())};

            if (integerType.IsSigned()) {
                return builder.CreateFPToSI(
                        value, integerType.GetLLVMType(), "float_to_signed_int"
                );
            }

            return builder.CreateFPToUI(
                    value, integerType.GetLLVMType(), "float_to_unsigned_int"
            );
        }

        throw diagnostics::FatalCompilerError{};
    }

    void PrintTo(ValueType const &type, std::ostream *os) {
        std::visit([&os](auto const &t) { PrintTo(t, os); }, type.GetType());
    }

    ValueType
    UsualArithmeticConversions(ValueType const &lhs, ValueType const &rhs) {
        if (!lhs.IsArithmetic() || !rhs.IsArithmetic())
            // TODO: Use FatalCompilerError with span info
            throw std::runtime_error{"must be arithmetic types"};

        // TODO: If one operand has decimal floating type, the other operand shall not have standard floating, complex, or imaginary type.
        // TODO: First, if the type of either operand is _Decimal128, the other operand is converted to _Decimal128.
        // TODO: Otherwise, if the type of either operand is _Decimal64, the other operand is converted to _Decimal64
        // TODO: Otherwise, if the type of either operand is _Decimal32, the other operand is converted to _Decimal32.
        // TODO: Otherwise, if the corresponding real type of either operand is long double, the other operand is converted, without change of type domain, to a type whose corresponding real type is long double.
        // TODO: Otherwise, if the corresponding real type of either operand is double, the other operand is converted, without change of type domain, to a type whose corresponding real type is double.
        // TODO: Otherwise, if the corresponding real type of either operand is float, the other operand is converted, without change of type domain, to a type whose corresponding real type is float.
        // TODO: Otherwise, if any of the two types is an enumeration, it is converted to its underlying type.

        if (std::holds_alternative<IntegerType>(lhs.GetType()) &&
            std::holds_alternative<IntegerType>(rhs.GetType())) {
            auto const &lhsIntType{std::get<IntegerType>(lhs.GetType())};
            auto const &rhsIntType{std::get<IntegerType>(rhs.GetType())};

            return ValueType{lhsIntType.UsualArithmeticConversion(rhsIntType)};
        }

        if (std::holds_alternative<FloatingType>(lhs.GetType()) !=
            std::holds_alternative<FloatingType>(rhs.GetType())) {
            auto const theFloatType{
                    std::holds_alternative<FloatingType>(lhs.GetType())
                            ? std::get<FloatingType>(lhs.GetType())
                            : std::get<FloatingType>(rhs.GetType())
            };
            auto const theIntType{
                    std::holds_alternative<FloatingType>(lhs.GetType())
                            ? std::get<IntegerType>(rhs.GetType())
                            : std::get<IntegerType>(lhs.GetType())
            };

            return ValueType{theIntType.UsualArithmeticConversion(theFloatType)
            };
        }

        if (std::holds_alternative<FloatingType>(lhs.GetType()) &&
            std::holds_alternative<FloatingType>(rhs.GetType())) {
            auto const &lhsFloatType{std::get<FloatingType>(lhs.GetType())};
            auto const &rhsFloatType{std::get<FloatingType>(rhs.GetType())};

            return ValueType{
                    lhsFloatType.UsualArithmeticConversion(rhsFloatType)
            };
        }

        throw std::runtime_error{"TODO: Implement UsualArithmeticConversions"};
    }
}// namespace jcc::parsing_sema::types
