#include "../../../hooks.hpp"

//related to FireBullet
static bool shot_is_from_local{}; 
static bool show_tracers{};

MAKE_HOOK(
	C_TFPlayer_FireBullet,
	s::C_TFPlayer_FireBullet.get(),
	void,
	void* rcx, C_TFWeaponBase* weapon,
	FireBulletsInfo_t& info, bool do_effects, int damage_type, int custom_damage_type)
{
	shot_is_from_local = false;

	if (!cfg::tracers_active) {
		return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
	}

	const C_TFPlayer* const local{ ec->getLocal() };

	if (!local) {
		return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
	}

	if (reinterpret_cast<C_TFPlayer*>(rcx) == local) {
		shot_is_from_local = true;
	}

	else {
		return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
	}

	show_tracers = false;

	static int last_tick_count{};
	static int last_bullet_amount{};
	static int shot_count{};

	if (!weapon || weapon != ec->getWeapon() || !i::pred->m_bFirstTimePredicted) {
		shot_count = 0;
		return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
	}

	int bullets_per_shot{ weapon->m_pWeaponInfo()->GetWeaponData(0).m_nBulletsPerShot };

	if (bullets_per_shot >= 1) {
		tf_utils::attribHookValue(bullets_per_shot, "mult_bullets_per_shot", weapon);
	}

	else {
		bullets_per_shot = 1;
	}

	if (bullets_per_shot != last_bullet_amount) {
		shot_count = 0;
		last_bullet_amount = bullets_per_shot;
		return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
	}

	if (last_tick_count == engine_pred->current_prediction_cmd)
	{
		shot_count++;

		if (shot_count >= bullets_per_shot) {
			return CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
		}
	}

	shot_count = 0;
	last_tick_count = engine_pred->current_prediction_cmd;

	show_tracers = true;

	info.m_iTracerFreq = 1;

	CALL_ORIGINAL(rcx, weapon, info, do_effects, damage_type, custom_damage_type);
}
//related to FireBullet
MAKE_HOOK(
	UTIL_ParticleTracer,
	s::UTIL_ParticleTracer.get(),
	void,
	const char* tracer_effect_name, const Vector& start_pos, const Vector& end_pos, int ent_index, int attachment_index, bool whiz_effects)
{
	if (!cfg::tracers_active || !shot_is_from_local) {
		return CALL_ORIGINAL(tracer_effect_name, start_pos, end_pos, ent_index, attachment_index, whiz_effects);
	}

	if (!show_tracers) {
		return;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	if (cfg::tracers_type == 0)
	{
		Color color{ cfg::tracers_color };

		if (cfg::tracers_mode == 1) {
			color = utils::rainbow(i::global_vars->curtime * cfg::tracers_rainbow_speed, 255);
		}

		vis_utils->fireBeam(start_pos, end_pos, color, cfg::tracers_fade);
	}

	else
	{
		auto getTracerName = [&]() -> const char*
			{
				const int team{ local->m_iTeamNum() };

				switch (cfg::tracers_type)
				{
				case 1: {
					return "dxhr_sniper_rail"; // white machina
				}

				case 2: {
					return team == TF_TEAM_RED ? "dxhr_sniper_rail_red" : "dxhr_sniper_rail_blu"; // red/blue
				}

				case 3: {
					return team == TF_TEAM_RED ? "bullet_tracer_raygun_red" : "bullet_tracer_raygun_blue"; // red/blue
				}

				default: {
					break;
				}
				}

				return nullptr;
			};

		const char* const tracer_name{ getTracerName() };

		if (!tracer_name) {
			return;
		}

		CALL_ORIGINAL(tracer_name, start_pos, end_pos, ent_index, attachment_index, whiz_effects);
	}
}
//related to FireBullet
MAKE_HOOK(
	WeaponID_IsSniperRifle,
	s::WeaponID_IsSniperRifle.get(),
	bool,
	int weaponID)
{
	if (cfg::tracers_active && shot_is_from_local) {
		return false;
	}

	return CALL_ORIGINAL(weaponID);
}