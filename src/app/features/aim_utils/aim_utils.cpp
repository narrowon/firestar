#include "aim_utils.hpp"

#include "../cfg.hpp"

//s: fixes isAttacking returning true when no ammo (reloading)
void AimUtils::fixAttackButtons(CUserCmd *const cmd)
{
	if (!cmd || !(cmd->buttons & IN_ATTACK)) {
		return;
	}

	C_TFWeaponBase *const weapon{ ec->getWeapon() };

	if (!weapon 
		|| tf_utils::isWeaponMelee(weapon)
		|| weapon->HasPrimaryAmmoForShot()
		|| weapon->GetWeaponID() == TF_WEAPON_LASER_POINTER) {
		return;
	}

	const CTFWeaponInfo *const weapon_info{ weapon->m_pWeaponInfo() };

	if (!weapon_info || weapon_info->GetWeaponData(0).m_nBulletsPerShot <= 0) {
		return;
	}

	if (!isAttacking(cmd, i::global_vars->curtime)) {
		return;
	}

	cmd->buttons &= ~IN_ATTACK;
}

//s: fixes crittokens going up too fast when inattack1&2 is held (not rly needed)
void AimUtils::fixMinigunButtons(CUserCmd *const cmd)
{
	if (!cmd || !(cmd->buttons & IN_ATTACK)) {
		return;
	}

	C_TFWeaponBase *const weapon{ ec->getWeapon() };

	if (!weapon 
		|| weapon->GetWeaponID() != TF_WEAPON_MINIGUN
		|| weapon->cast<C_TFMinigun>()->m_iWeaponState() != AC_STATE_FIRING) {
		return;
	}

	cmd->buttons &= ~IN_ATTACK2;
}

bool AimUtils::isAttacking(CUserCmd *const cmd, float curtime, const bool melee_no_swing)
{
	if (!cmd) {
		return false;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return false;
	}

	C_TFWeaponBase *const weapon{ ec->getWeapon() };

	if (!weapon) {
		return false;
	}
	
	const bool can_primary_attack {
		(curtime >= local->m_flNextAttack()) && (curtime >= weapon->m_flNextPrimaryAttack())
	};
	
	if (tf_utils::isWeaponMelee(weapon))
	{
		if (melee_no_swing || weapon->GetWeaponID() == TF_WEAPON_KNIFE) {
			 return (cmd->buttons & IN_ATTACK) && can_primary_attack;
		}

		else {
			return (fabsf(weapon->m_flSmackTime() - curtime) < i::global_vars->interval_per_tick * 2.0f);
		}
	}

	switch (weapon->GetWeaponID())
	{
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			if (const float charge_begin_time{ weapon->cast<C_TFPipebombLauncher>()->m_flChargeBeginTime() })
			{
				const float charge{ curtime - charge_begin_time };
				const float max_charge{ tf_utils::attribHookValue(4.0f, "stickybomb_charge_rate", weapon) };

				if ((charge > max_charge)
					&& (cmd->buttons & IN_ATTACK)
					&& (tf_globals::last_cmd.buttons & IN_ATTACK)
					&& can_primary_attack) {
					return true;
				}
			}

			return (!(cmd->buttons & IN_ATTACK) && (tf_globals::last_cmd.buttons & IN_ATTACK)) && can_primary_attack;
		}

		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC: {
			return (!(cmd->buttons & IN_ATTACK) && (tf_globals::last_cmd.buttons & IN_ATTACK)) && can_primary_attack;
		}

		case TF_WEAPON_FLAME_BALL:
		{
			const float m_flItemChargeMeter{ *reinterpret_cast<float *>(reinterpret_cast<uintptr_t>(local) + 0x2104) };

			//s: should be using 100.0f but that fucks with it
			return (cmd->buttons & IN_ATTACK) && (curtime >= local->m_flNextAttack()) && m_flItemChargeMeter >= 80.0f;
		}

		default:
		{
			if (weapon->m_iItemDefinitionIndex() == Soldier_m_TheBeggarsBazooka) {
				return weapon->m_iReloadMode() == TF_RELOAD_START && can_primary_attack && weapon->HasPrimaryAmmoForShot();
			}

			if (weapon->GetWeaponID() == TF_WEAPON_MINIGUN && weapon->cast<C_TFMinigun>()->m_iWeaponState() < 2) {
				return false;
			}

			return (cmd->buttons & IN_ATTACK) && can_primary_attack;
		}
	}
}

void AimUtils::setTickCount(CUserCmd *const cmd, C_BaseEntity *const ent, const LagRecord *const lr)
{
	if (!cmd || !ent || ent->GetClassId() != class_ids::CTFPlayer || !isAttacking(cmd, i::global_vars->curtime)) {
		return;
	}

	C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

	if (!pl || pl->deadflag()) {
		return;
	}

	cmd->tick_count = tf_utils::timeToTicks((lr ? lr->sim_time : pl->m_flSimulationTime()) + tf_utils::getLerp());
}

bool AimUtils::getMeleeInfo(float& swing_range, vec3& hull_mins, vec3& hull_maxs)
{
	C_TFPlayer* const local{ ec->getLocal() };

	if (!local) {
		return false;
	}

	C_TFWeaponBase* const weapon{ ec->getWeapon() };

	if (!weapon || !tf_utils::isWeaponMelee(weapon)) {
		return false;
	}

	swing_range = weapon->GetSwingRange();

	hull_mins = { -18.0f, -18.0f, -18.0f };
	hull_maxs = { 18.0f, 18.0f, 18.0f };

	const float hull_scale{ tf_utils::attribHookValue(1.0f, "melee_bounds_multiplier", weapon) };

	hull_mins *= hull_scale;
	hull_maxs *= hull_scale;

	const float model_scale{ local->m_flModelScale() };

	//l3: for some reason midgets still get normal people's range and hull, where is equality ???
	if (model_scale > 1.0f) {
		swing_range *= model_scale;
		hull_mins *= model_scale;
		hull_maxs *= model_scale;
	}

	swing_range = tf_utils::attribHookValue(swing_range, "melee_range_multiplier", weapon);

	return true;
}
