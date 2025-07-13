#pragma once

#include "../../../game/game.hpp"

class Anims final
{
public:
	void postDataUpdate(C_BasePlayer *const base_pl);
};

MAKE_UNIQUE(Anims, anims);