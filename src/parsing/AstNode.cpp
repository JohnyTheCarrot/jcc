//
// Created by johny on 11/8/23.
//
#include "AstNode.h"

std::string parsing::AstNode::Indent(size_t depth) {
    std::string indent{};
    for (size_t i = 0; i < depth; ++i) {
        indent += "  ";
    }
    return indent;
}
