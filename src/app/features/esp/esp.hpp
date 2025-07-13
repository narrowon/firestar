#pragma once

#include "../../../game/game.hpp"

class ESP final
{
private:
	void drawName(const float x, const float y, const float w, const float h, std::string_view name, const Color clr, size_t *text_offset);
	void drawHpBar(const float x, const float y, const float w, const float h, float hp, const float max_hp, const Color clr, const bool outline = true);
	void drawBox(const float x, const float y, const float w, const float h, const Color clr);

public:
	void run();
};

MAKE_UNIQUE(ESP, esp);