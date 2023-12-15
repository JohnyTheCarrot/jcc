//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTSTORAGECLASSSPECIFIER_H
#define JCC_ASTSTORAGECLASSSPECIFIER_H

#include "../../AstNode.h"

namespace parsing {

	struct AstStorageClassSpecifier final : public AstNode {
		enum class StorageClassSpecifierType { Typedef, Extern, Static, ThreadLocal, Auto, Register };

		AstStorageClassSpecifier(StorageClassSpecifierType storageClassSpecifier)
			: AstNode(AstNodeType::StorageClassSpecifier, Hierarchies::Declaration)
			, _storageClassSpecifier(storageClassSpecifier) {}

		[[nodiscard]]
		static std::optional<AstStorageClassSpecifier> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		StorageClassSpecifierType _storageClassSpecifier;
	};

}// namespace parsing

#endif//JCC_ASTSTORAGECLASSSPECIFIER_H
