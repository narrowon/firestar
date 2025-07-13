#pragma once

#include "../../../game/game.hpp"

class AutoDetonate final
{
private:
	float m_live_time{}; // time until sticky is live (can be detonated) - see CTFGrenadePipebombProjectile::GetLiveTime

private:
	float getStickyRadius(C_TFPipebombLauncher *const launcher, C_TFGrenadePipebombProjectile *const sticky) const;
	bool canSee(C_TFPipebombLauncher *const launcher, C_TFGrenadePipebombProjectile *const sticky, C_BaseEntity *const target) const;
	void detonateSticky(CUserCmd *const cmd, C_TFPlayer *const local, C_TFGrenadePipebombProjectile *const sticky);

public:
	void run(CUserCmd *const cmd);
};

MAKE_UNIQUE(AutoDetonate, auto_detonate);