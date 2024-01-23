//
// Created by Tuur Martens on 15/01/2024.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H
#include "Config.h"

class Span final {
	size_t m_lineNumberStart{}, m_lineNumberEnd{};
	size_t m_characterIndexStart{}, m_characterIndexEnd{};
};

#endif//JCC_SPAN_H
