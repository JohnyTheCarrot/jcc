//
// Created by johny on 11/8/23.
//
#include "AstNode.h"

std::string parsing::AstNode::Indent(size_t depth) {
	std::string indent{};
	for (size_t i = 0; i < depth; ++i) { indent += "  "; }
	return indent;
}

// https://stackoverflow.com/a/42711458/5119970
std::ostream &WriteStringExact(std::ostream &out, const std::string &str) {
	for (auto ch: str) {
		switch (ch) {
			case '\'':
				out << "\\'";
				break;

			case '\"':
				out << "\\\"";
				break;

			case '\?':
				out << "\\?";
				break;

			case '\\':
				out << "\\\\";
				break;

			case '\a':
				out << "\\a";
				break;

			case '\b':
				out << "\\b";
				break;

			case '\f':
				out << "\\f";
				break;

			case '\n':
				out << "\\n";
				break;

			case '\r':
				out << "\\r";
				break;

			case '\t':
				out << "\\t";
				break;

			case '\v':
				out << "\\v";
				break;

			default:
				out << ch;
		}
	}

	return out;
}
