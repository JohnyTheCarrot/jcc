#ifndef JCC_COMPILER_DATA_TYPES_H
#define JCC_COMPILER_DATA_TYPES_H

#include <cstdint>
#include <limits>
#include <string>

namespace CompilerDataTypeInfo {
	template<typename Type>
	struct TypeInfo final : std::numeric_limits<Type> {
		using TYPE                   = Type;
		static constexpr size_t MASK = std::numeric_limits<std::make_unsigned_t<Type>>::max();
	};

	using INT_LEAST8_T  = TypeInfo<int8_t>;
	using UINT_LEAST8_T = TypeInfo<uint8_t>;

	using INT_LEAST16_T  = TypeInfo<int16_t>;
	using UINT_LEAST16_T = TypeInfo<uint16_t>;

	using INT_LEAST32_T  = TypeInfo<int32_t>;
	using UINT_LEAST32_T = TypeInfo<uint32_t>;

	using INT_LEAST64_T  = TypeInfo<int64_t>;
	using UINT_LEAST64_T = TypeInfo<uint64_t>;

	using CHAR     = TypeInfo<char>;
	using WCHAR_T  = CHAR;
	using CHAR16_T = INT_LEAST16_T;// equal per the standard
	using CHAR32_T = INT_LEAST32_T;// equal per the standard
	using INT      = TypeInfo<int16_t>;
}// namespace CompilerDataTypeInfo

namespace CompilerDataTypes {
	using Char            = CompilerDataTypeInfo::CHAR::TYPE;
	using String          = std::basic_string<CompilerDataTypeInfo::CHAR::TYPE>;
	using StringView      = std::basic_string_view<CompilerDataTypeInfo::CHAR::TYPE>;
	using StringConstIter = String::const_iterator;

#define COMP_STRING(str) reinterpret_cast<const CompilerDataTypeInfo::CHAR::TYPE *>(str)
};// namespace CompilerDataTypes

#endif//JCC_COMPILER_DATA_TYPES_H
