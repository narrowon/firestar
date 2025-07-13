#pragma once

#include "../../../game/game.hpp"

class Misc final
{
public:
	void bhop(CUserCmd *const cmd);
	auto prediction_path() -> void;
	void autostrafe(CUserCmd *const cmd);
	void tauntSpin(CUserCmd *const cmd);
	void sentryWiggler(CUserCmd *const cmd);
	void fastStop(CUserCmd *const cmd);
	void edgeJump(CUserCmd *const cmd);
	void noisemakerSpam();
	void mvmRespawn();
	void autoMvmReadyUp();
	void autoDisguise();

public:
	void paintFOVCircle();
};

MAKE_UNIQUE(Misc, misc);
