//
// Created by Tuur Martens on 15/01/2024.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H
#include "Config.h"

struct Span final {
	std::size_t m_LineNumberStart{}, m_LineNumberEnd{};
	std::size_t m_CharacterIndexStart{}, m_CharacterIndexEnd{};
};

#endif//JCC_SPAN_H
