#include "auto_detonate.hpp"

#include "../cfg.hpp"
#include "../trace_utils/trace_utils.hpp"
#include "../aim_utils/aim_utils.hpp"

float AutoDetonate::getStickyRadius(C_TFPipebombLauncher *const launcher, C_TFGrenadePipebombProjectile *const sticky) const
{
	/*static ConVar *const tf_sticky_radius_ramp_time{ i::cvar->FindVar("tf_sticky_radius_ramp_time") };
	static ConVar *const tf_sticky_airdet_radius { i::cvar->FindVar("tf_sticky_airdet_radius") };
	static ConVar *const tf_grenadelauncher_livetime{ i::cvar->FindVar("tf_grenadelauncher_livetime") };

	float radius_mod{ 1.0f };

	if (sticky->m_iType() == TF_GL_MODE_REMOTE_DETONATE)
	{
		if (sticky->m_bTouched() == false)
		{
			const float arm_time{ tf_grenadelauncher_livetime->GetFloat() };

			radius_mod *= math::remap(
				i::global_vars->curtime - sticky->m_flCreationTime(),
				arm_time, arm_time + tf_sticky_radius_ramp_time->GetFloat(),
				tf_sticky_airdet_radius->GetFloat(), 1.0f
			);
		}
	}

	return tf_utils::attribHookValue(sticky->m_DmgRadius(), "mult_explosion_radius", launcher) * radius_mod;*/

	//above gives 146..

	if (!launcher || !sticky) {
		return 0.0f;
	}

	return sticky->m_bTouched() ? 170.0f : 150.0f;
}

bool AutoDetonate::canSee(C_TFPipebombLauncher *const launcher, C_TFGrenadePipebombProjectile *const sticky, C_BaseEntity *const target) const
{
	if (!launcher || !sticky || !target) {
		return false;
	}

	const vec3 target_center{ target->GetCenter() };
	const vec3 sticky_center{ sticky->GetCenter() };

	const float radius{ getStickyRadius(launcher, sticky) };

	if (sticky_center.distTo(target_center) > radius) {
		return false;
	}

	trace_t trace{};

	tf_utils::trace(sticky_center, target_center, &trace_filters::world, &trace, MASK_SOLID & ~CONTENTS_GRATE);

	return trace.fraction > 0.99f;
}

void AutoDetonate::detonateSticky(CUserCmd *const cmd, C_TFPlayer *const local, C_TFGrenadePipebombProjectile *const sticky)
{
	cmd->buttons |= IN_ATTACK2;

	if (sticky->m_bDefensiveBomb()) {
		const vec3 target_pos{ math::calcAngle(local->GetEyePos(), sticky->GetCenter()) };
		tf_utils::fixMovement(cmd, target_pos);
		cmd->viewangles = target_pos;
	}
}

void AutoDetonate::run(CUserCmd *const cmd)
{
	if (!cfg::auto_detonate_active
		|| !cmd
		|| (cfg::auto_detonate_mode == 0 && !input_tick->getKeyInGame(cfg::auto_detonate_key).held)) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag() || local->m_iClass() != TF_CLASS_DEMOMAN) {
		return;
	}

	C_TFPipebombLauncher *const weapon{ local->Weapon_OwnsThisID(TF_WEAPON_PIPEBOMBLAUNCHER)->cast<C_TFPipebombLauncher>() };

	if (!weapon) {
		return;
	}

	static ConVar *const tf_grenadelauncher_livetime{ i::cvar->FindVar("tf_grenadelauncher_livetime") };

	float live_time{ tf_utils::attribHookValue(tf_grenadelauncher_livetime->GetFloat(), "sticky_arm_time", weapon) };

	if (local->GetCarryingRuneType() == RUNE_HASTE) {
		live_time *= 0.5f;
	}

	else if (local->GetCarryingRuneType() == RUNE_KING || local->InCond(TF_COND_KING_BUFFED)) {
		live_time *= 0.75f;
	}

	m_live_time = live_time;

	std::vector<C_TFGrenadePipebombProjectile *> stickies{};

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PROJECTILES_LOCAL))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_BaseEntity *const proj{ ent->cast<C_BaseEntity>() };

		if (!proj || proj->IsDormant() || proj->GetClassId() != class_ids::CTFGrenadePipebombProjectile) {
			continue;
		}

		C_TFGrenadePipebombProjectile *const sticky{ proj->cast<C_TFGrenadePipebombProjectile>() };

		if (!sticky || sticky->m_iType() == TF_GL_MODE_REMOTE_DETONATE_PRACTICE) {
			continue;
		}

		if (i::global_vars->curtime < sticky->m_flCreationTime() + m_live_time) {
			continue;
		}

		stickies.push_back(sticky);
	}

	if (stickies.empty()) {
		return;
	}

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PLAYERS_ENEMIES))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

		if (!pl || pl->deadflag() || pl->IsInvisible() || pl->IsInvulnerable()) {
			continue;
		}

		for (C_TFGrenadePipebombProjectile *const sticky : stickies)
		{
			if (!canSee(weapon, sticky, pl)) {
				continue;
			}

			detonateSticky(cmd, local, sticky);

			return;
		}
	}

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::BUILDINGS_ENEMIES))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_BaseObject *const obj{ ent->cast<C_BaseObject>() };

		if (!obj || (obj->m_iHealth() <= 0) || obj->m_bCarried()) {
			continue;
		}

		for (C_TFGrenadePipebombProjectile *const sticky : stickies)
		{
			if (!canSee(weapon, sticky, obj)) {
				continue;
			}

			detonateSticky(cmd, local, sticky);

			return;
		}
	}
}