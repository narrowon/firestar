#include "aimbot.hpp"

#include "aimbot_hitscan/aimbot_hitscan.hpp"
#include "aimbot_melee/aimbot_melee.hpp"
#include "aimbot_projectile/aimbot_projectile.hpp"

#include "../aim_utils/aim_utils.hpp"
#include "../cfg.hpp"

void Aimbot::run(CUserCmd *const cmd)
{
	if (!cfg::aimbot_active
		|| !cmd
		|| (cfg::aimbot_mode == 0 && !input_tick->getKeyInGame(cfg::aimbot_key).held)
		|| (cfg::aimbot_mode == 1 && !aim_utils->isAttacking(cmd, i::global_vars->curtime))) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local
		|| local->deadflag()
		|| local->InCond(TF_COND_STUNNED)
		|| local->InCond(TF_COND_TAUNTING)) {
		return;
	}

	C_TFWeaponBase *const wep{ ec->getWeapon() };

	if (!wep) {
		return;
	}

	const int weapon_id{ wep->GetWeaponID() };

	if (weapon_id == TF_WEAPON_MEDIGUN
		|| weapon_id == TF_WEAPON_BUILDER
		|| weapon_id == TF_WEAPON_PDA_ENGINEER_BUILD
		|| weapon_id == TF_WEAPON_PDA_ENGINEER_DESTROY
		|| weapon_id == TF_WEAPON_PDA_SPY) {
		return;
	}

	if (tf_utils::isWeaponHitscan(wep)) {
		aimbot_hitscan->run(cmd, local, wep);
	}

	if (tf_utils::isWeaponMelee(wep)) {
		aimbot_melee->run(cmd, local, wep);
	}

	if (tf_utils::isWeaponProjectile(wep)) {
		aimbot_projectile->run(cmd, local, wep);
	}
}