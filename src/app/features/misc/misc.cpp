#include "misc.hpp"

#include "../cfg.hpp"

void Misc::bhop(CUserCmd *const cmd)
{
	if (!cfg::misc_bhop || !cmd) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local
		|| local->deadflag()
		|| (local->m_nWaterLevel() > WL_Feet)
		|| (local->m_MoveType() != MOVETYPE_WALK)) {
		return;
	}

	static bool block_jump{};

	if (cmd->buttons & IN_JUMP)
	{
		if (!block_jump && !(local->m_fFlags() & FL_ONGROUND)) {
			cmd->buttons &= ~IN_JUMP;
		}

		else if (block_jump) {
			block_jump = false;
		}
	}

	else if (!block_jump) {
		block_jump = true;
	}
}
auto Misc::prediction_path() -> void
{
	if (!cfg::prediction_path_enable || tf_globals::pred_path.size() < 2) {
		return;
	}

	if (i::global_vars->curtime >= tf_globals::pred_path_expire_time) {
		tf_globals::pred_path.clear();
		return;
	}

	vec2 Past{}, Future{};

	for (size_t n = 1; n < tf_globals::pred_path.size(); n++) {
		if (draw->worldToScreen(tf_globals::pred_path[n - 1], Past)) {
			if (draw->worldToScreen(tf_globals::pred_path[n], Future)) {
				draw->line(vec2(Past.x, Past.y), vec2(Future.x, Future.y), cfg::prediction_path_color);
			}
		}
	}
}
void Misc::autostrafe(CUserCmd *const cmd)
{
	if (!cfg::auto_strafe_active || !cmd) {
		return;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local
		|| local->deadflag()
		|| (local->m_fFlags() & FL_ONGROUND)
		|| (local->m_nWaterLevel() > WL_Feet)
		|| (local->m_MoveType() != MOVETYPE_WALK)
		|| (!(tf_globals::last_cmd.buttons & IN_JUMP) && (cmd->buttons & IN_JUMP))) {
		return;
	}

	if (!(cmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))) {
		return;
	}

	const float f_move{ cmd->forwardmove };
	const float s_move{ cmd->sidemove };

	vec3 view_forward{};
	vec3 view_right{};
	math::angleVectors(i::engine->GetViewAngles(), &view_forward, &view_right, nullptr);

	view_forward.z = 0.0f;
	view_right.z = 0.0f;

	view_forward.normalize();
	view_right.normalize();

	vec3 cur_dir{};
	math::vectorAngles(local->m_vecVelocity(), cur_dir);

	vec3 wish_dir{};
	math::vectorAngles({ (view_forward.x * f_move) + (view_right.x * s_move), (view_forward.y * f_move) + (view_right.y * s_move), 0.0f }, wish_dir);

	const float dir_delta{ math::normalizeAngle(wish_dir.y - cur_dir.y) };
	const float delta_scale{ math::remap(cfg::auto_strafe_smooth, 0.0f, 1.0f, 0.9f, 1.0f) };
	const float rotation{ math::degToRad((dir_delta > 0.0f ? -90.0f : 90.f) + (dir_delta * delta_scale)) };

	const float rot_cos{ cosf(rotation) };
	const float rot_sin{ sinf(rotation) };

	cmd->forwardmove = (rot_cos * f_move) - (rot_sin * s_move);
	cmd->sidemove = (rot_sin * f_move) + (rot_cos * s_move);
}

void Misc::tauntSpin(CUserCmd *const cmd)
{
	if (!cmd || !cfg::misc_taunts_slide) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag() || !local->InCond(TF_COND_TAUNTING)) {
		return;
	}

	Vector angles{ cmd->viewangles };
	float final_pitch{ angles.x };

	if (cfg::misc_taunts_control)
	{
		const TauntIndex_t tauntIndex{ static_cast<TauntIndex_t>(local->m_iTauntItemDefIndex()) };

		const float backwardsPitch{ tf_utils::tauntMovesForward(tauntIndex) ? 91.f : 0.f };
		const float forwardsPitch{ (cmd->buttons & IN_FORWARD) ? 0.0f : 90.f };

		final_pitch = (cmd->buttons & IN_BACK) ? backwardsPitch : forwardsPitch;

		if (cmd->buttons & IN_MOVELEFT) {
			cmd->sidemove = -450.0f;
		}

		else if (cmd->buttons & IN_MOVERIGHT) {
			cmd->sidemove = 450.0f;
		}
	}

	if (input_tick->getKeyInGame(cfg::misc_taunts_spin_toggle_key).pressed) {
		cfg::misc_taunts_spin = !cfg::misc_taunts_spin;
	}
	
	static float taunt_spin_yaw{ angles.y };

	if (cfg::misc_taunts_follow_camera) {
		angles.y = i::engine->GetViewAngles().y;
	}

	if (cfg::misc_taunts_spin)
	{
		const float speed{ fabsf(cfg::misc_taunts_spin_speed) };
		const float time{ i::global_vars->curtime };
		const float interval{ i::global_vars->interval_per_tick * math::pi() * 2.0f };

		float yaw{ cfg::misc_taunts_spin_speed };

		auto randomFloat = [](float min, float max) {
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
		};

		auto triangleWave = [](float x, float t, float a)
		{
			if (t == 0.0f) {
				return x;
			}

			const float phase{ fmod(x, t) / t };
			const float value{ (2.0f * abs(2.f * (phase - 0.5f)) - 1.0f) * a };

			return value;
		};

		switch (cfg::misc_taunts_spin_mode)
		{
			case 1: {
				yaw = randomFloat(-speed, speed);
				break;
			}

			case 2: {
				yaw = sinf(time) * cfg::misc_taunts_spin_speed;
				break;
			}

			case 3: {
				yaw = triangleWave(time, interval, speed);
				break;
			}

			default: {
				break;
			}
		}

		angles.y = math::normalizeAngle(taunt_spin_yaw += yaw);
	}

	else {
		taunt_spin_yaw = angles.y;
	}

	tf_utils::fixMovement(cmd, angles);

	cmd->viewangles.y = angles.y;
	cmd->viewangles.x = final_pitch;
}

void Misc::sentryWiggler(CUserCmd *const cmd)
{
	if (!cfg::auto_sentry_wiggler
		|| !cmd
		|| input_tick->getKeyInGame(cfg::auto_sentry_wiggler_panic_key).held)
	{
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return;
	}
	
	C_ObjectSentrygun *const sentry{ local->m_hGroundEntity().Get()->cast<C_ObjectSentrygun>() };

	if (!sentry || sentry->GetClassId() != class_ids::CObjectSentrygun) {
		return;
	}

	if (!cfg::auto_sentry_wiggler_self_rodeo && sentry->m_hBuilder().Get() == local) {
		return;
	}

	vec3 muzzle_position{};

	if (sentry->m_iUpgradeLevel() < 2) {
		vec3 ang{};
		sentry->GetAttachment(sentry->LookupAttachment("muzzle"), muzzle_position, ang);
	}

	else
	{
		vec3 ang{};

		vec3 left_muzzle_position{};
		sentry->GetAttachment(sentry->LookupAttachment("muzzle_l"), left_muzzle_position, ang);

		vec3 right_muzzle_position{};
		sentry->GetAttachment(sentry->LookupAttachment("muzzle_r"), right_muzzle_position, ang);

		muzzle_position = (left_muzzle_position + right_muzzle_position) * 0.5f;
	}

	const vec3 sentry_mins{ sentry->m_vecMins() };
	const vec3 sentry_maxs{ sentry->m_vecMaxs() };
	const vec3 sentry_center{ sentry->GetRenderCenter() };
	const vec3 sentry_origin{ sentry->GetAbsOrigin() };

	const float top{ sentry_origin.z + sentry_maxs.z };

	muzzle_position.z = top;

	const vec3 walk_position{ sentry_center.x, sentry_center.y, top };
	const vec3 muzzle_angle { math::calcAngle(walk_position, muzzle_position) };

	vec3 forward{};
	math::angleVectors(muzzle_angle, &forward);

	const float distance{ sentry_center.distTo(muzzle_position) * cfg::auto_sentry_wiggler_distance};

	vec3 end_position{ walk_position + (forward * -distance) };

	end_position.x = std::clamp(end_position.x, sentry_center.x + sentry_mins.x, sentry_center.x + sentry_maxs.x);
	end_position.y = std::clamp(end_position.y, sentry_center.y + sentry_mins.y, sentry_center.y + sentry_maxs.y);
	end_position.z = top;

	tf_utils::walkTo(cmd, local->GetAbsOrigin(), end_position, 0.3f);
}

void Misc::paintFOVCircle()
{
	if (!cfg::aimbot_active || !cfg::fov_circle_active) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag() || !local->InFirstPersonView()) {
		return;
	}

	C_TFWeaponBase *const weapon{ ec->getWeapon() };

	if (!weapon) {
		return;
	}

	const int weapon_id{ weapon->GetWeaponID() };

	if (weapon_id == TF_WEAPON_MEDIGUN
		|| weapon_id == TF_WEAPON_BUILDER
		|| weapon_id == TF_WEAPON_PDA_ENGINEER_BUILD
		|| weapon_id == TF_WEAPON_PDA_ENGINEER_DESTROY
		|| weapon_id == TF_WEAPON_PDA_SPY) {
		return;
	}

	float aimbot_fov{};

	if (tf_utils::isWeaponHitscan(weapon))
	{
		if (!cfg::aimbot_hitscan_active || cfg::aimbot_hitscan_mode != 0) {
			return;
		}

		aimbot_fov = cfg::aimbot_hitscan_fov;
	}

	if (tf_utils::isWeaponMelee(weapon))
	{
		if (!cfg::aimbot_melee_active || cfg::aimbot_melee_mode != 0) {
			return;
		}

		aimbot_fov = cfg::aimbot_melee_fov;
	}

	if (tf_utils::isWeaponProjectile(weapon))
	{
		if (!cfg::aimbot_projectile_active || (cfg::aimbot_projectile_mode != 0 && cfg::aimbot_projectile_mode != 1)) {
			return;
		}

		aimbot_fov = cfg::aimbot_projectile_fov;
	}

	const float radius{ tanf(math::degToRad(aimbot_fov) * 0.5f) / tanf(math::degToRad(local->GetFOV()) * 0.5f) * draw->getScreenSize().x };

	Color color{ cfg::fov_circle_color };

	color.a = static_cast<uint8_t>(cfg::fov_circle_alpha * 255.0f);

	if (cfg::fov_circle_style == 0) {
		draw->circle(draw->getScreenSize() * 0.5f, radius, color);
	}

	if (cfg::fov_circle_style == 1)
	{
		static float rotation{};

		rotation += cfg::fov_circle_spin_speed * i::global_vars->frametime;

		if (rotation >= 2.0f * 3.14f) {
			rotation -= 2.0f * 3.14f;
		}

		const int segments{ 100 };
		
		for (int n{}; n < segments; n++)
		{
			if (n % cfg::fov_circle_spacing) {
				continue;
			}

			const float segment_angle{ 2.0f * 3.14f / static_cast<float>(segments) };

			const float angle_0{ n * segment_angle + rotation };
			const float angle_1{ (n + 1) * segment_angle + rotation };

			const vec2 circle_pos{ draw->getScreenSize() * 0.5f };

			const vec2 p0{ circle_pos.x + radius * cosf(angle_0), circle_pos.y + radius * sinf(angle_0) };
			const vec2 p1{ circle_pos.x + radius * cosf(angle_1), circle_pos.y + radius * sinf(angle_1) };

			draw->line(p0, p1, color);
		}
	}
}

void Misc::fastStop(CUserCmd *const cmd)
{
	if (!cfg::misc_faststop || !cmd) {
		return;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local
		|| local->deadflag()
		|| local->m_MoveType() != MOVETYPE_WALK
		|| !(local->m_fFlags() & FL_ONGROUND)
		|| local->m_vecVelocity().length2D() < 5.0f
		|| (cmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT)))
	{
		return;
	}

	const vec3 vel{ local->m_vecVelocity() };

	vec3 dir{};
	math::vectorAngles(vel, dir);

	dir.y = i::engine->GetViewAngles().y - dir.y;

	vec3 forward{};
	math::angleVectors(dir, &forward);

	const vec3 negated_dir{ forward * -vel.length() };

	cmd->forwardmove = negated_dir.x;
	cmd->sidemove = negated_dir.y;
}

void Misc::noisemakerSpam()
{
	if (input_tick->getKeyInGame(cfg::misc_noisemaker_spam_toggle).pressed) {
		cfg::misc_noisemaker_spam = !cfg::misc_noisemaker_spam;
	}

	if (!cfg::misc_noisemaker_spam) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || i::global_vars->curtime < local->m_flNextNoiseMakerTime()) {
		return;
	}

	KeyValues *const kv{ new KeyValues("use_action_slot_item_server") };

	i::engine->ServerCmdKeyValues(kv);
}

void Misc::mvmRespawn()
{
	if (!cfg::misc_mvm_instant_respawn) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || !local->deadflag()) {
		return;
	}

	KeyValues *const kv{ new KeyValues("MVM_Revive_Response") };

	kv->SetString("accepted", "1");

	i::engine->ServerCmdKeyValues(kv);
}

void Misc::autoMvmReadyUp()
{
	if (!cfg::auto_mvm_ready_up) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	C_TFGameRulesProxy *game_rules{ ec->getGameRules() };

	if (!game_rules) {
		return;
	}

	if (!game_rules->m_bPlayingMannVsMachine()
		|| !game_rules->m_bInWaitingForPlayers()
		|| game_rules->m_iRoundState() != GR_STATE_BETWEEN_RNDS) {
		return;
	}

	const int local_index{ local->entindex() };

	if (local_index < 0 || local_index >= 100) {
		return;
	}

	if (!game_rules->IsPlayerReady(local_index)) {
		i::engine->ClientCmd_Unrestricted("tournament_player_readystate 1");
	}
}

void Misc::autoDisguise()
{
	if (!cfg::auto_disguise) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local 
		|| local->deadflag() 
		|| local->m_iClass() != TF_CLASS_SPY 
		|| local->InCond(TF_COND_DISGUISED) 
		|| local->InCond(TF_COND_DISGUISING)) {
		return;
	}

	i::engine->ClientCmd_Unrestricted("lastdisguise");
}