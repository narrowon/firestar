#include "aimbot_melee.hpp"

#include "../../aim_utils/aim_utils.hpp"
#include "../../trace_utils/trace_utils.hpp"
#include "../../move_sim/move_sim.hpp"
#include "../../cfg.hpp"

bool AimbotMelee::getMeleeInfo(C_TFPlayer *const local, C_TFWeaponBase *const weapon, float &swing_range, vec3 &hull_mins, vec3 &hull_maxs)
{
	if (!local || !weapon) {
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

bool AimbotMelee::canSee(C_TFPlayer *const local, C_TFWeaponBase *const weapon, const Target &target, const vec3 *const local_pos_override, const bool skip_lr)
{
	if (!local || !target.m_ent) {
		return false;
	}

	vec3 forward{};
	math::angleVectors(target.m_ang_to, &forward);

	float swing_range{};

	vec3 hull_mins{};
	vec3 hull_maxs{};

	if (!getMeleeInfo(local, weapon, swing_range, hull_mins, hull_maxs)) {
		return false;
	}

	const vec3 trace_start{ local_pos_override ? *local_pos_override : local->GetEyePos() };
	const vec3 trace_end{ trace_start + (forward * swing_range) };

	trace_filters::melee_aim.m_ignore_ent = local;

	trace_t trace{};

	//s: called from swing pred, dont override origin
	if (skip_lr) {
		tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
	}

	else
	{
		if (const LagRecord *const lr{ target.m_lag_record }) {
			lrm->set(target.m_ent->cast<C_TFPlayer>(), lr);
			tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
			lrm->reset();
		}

		else {
			tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
		}
	}

	return trace.m_pEnt == target.m_ent;
}

bool AimbotMelee::swingPred(C_TFPlayer *const local, C_TFWeaponBase *const weapon, Target &target)
{
	if (!cfg::aimbot_melee_swing_pred
		|| !local
		|| !weapon
		|| weapon->GetWeaponID() == TF_WEAPON_KNIFE
		|| !target.m_ent
		|| !target.m_can_swing_pred) {
		return false;
	}

	auto getPositions = [](C_BaseEntity *const ent, std::vector<vec3> &out, const bool ignore_special_ability)
	{
		if (!ent) {
			return false;
		}

		if (ent->GetClassId() != class_ids::CTFPlayer) {
			out.push_back(ent->GetCenter());
			return true;
		}

		if (ent->cast<C_TFPlayer>()->m_vecVelocity().isZero()) {
			out.push_back(ent->m_vecOrigin());
			return true;
		}

		if (!move_sim->init(ent->cast<C_TFPlayer>(), ignore_special_ability)) {
			return false;
		}

		for (int n{}; n < tf_utils::timeToTicks(cfg::aimbot_melee_swing_pred_time); n++) {
			move_sim->tick();
			out.push_back(move_sim->origin());
		}

		move_sim->restore();

		return true;
	};

	if (m_local_positions.empty() && !getPositions(local, m_local_positions, true)) {
		return false;
	}

	std::vector<vec3> target_positions{};

	if (!getPositions(target.m_ent, target_positions, false)) {
		return false;
	}

	for (int n{}; n < tf_utils::timeToTicks(cfg::aimbot_melee_swing_pred_time); n++)
	{
		const size_t local_pos_idx{ std::clamp(static_cast<size_t>(n), 0ull, m_local_positions.size() - 1) };
		const size_t target_pos_idx{ std::clamp(static_cast<size_t>(n), 0ull, target_positions.size() - 1) };

		const AimUtils::SetAbsOrigin set_abs_origin{ target.m_ent, target_positions[target_pos_idx] };

		const vec3 local_pos{ m_local_positions[local_pos_idx] + local->m_vecViewOffset() };
		const vec3 target_pos{ target_positions[target_pos_idx] + vec3{ 0.0f, 0.0f, (target.m_ent->m_vecMins().z + target.m_ent->m_vecMaxs().z) * 0.7f } };

		if (canSee(local, weapon, target, &local_pos, true))
		{
			target.m_ang_to = math::calcAngle(local_pos, target_pos);
			target.m_pos = target_pos;
			target.m_was_swing_preded = true;

			return true;
		}
	}

	return false;
}

bool AimbotMelee::processTarget(C_TFPlayer *const local, C_TFWeaponBase *const weapon, Target &target)
{
	if (!local || !target.m_ent) {
		return false;
	}

	if (!canSee(local, weapon, target)) {
		return swingPred(local, weapon, target);
	}

	return true;
}

bool AimbotMelee::getTarget(C_TFPlayer *const local, C_TFWeaponBase *const weapon, Target &out)
{
	if (!local || !weapon) {
		return false;
	}

	std::vector<Target> targets{};

	const ECGroup player_group
	{
		tf_utils::attribHookValue(0.0f, "speed_buff_ally", weapon) > 0.0f
		? ECGroup::PLAYERS_ALL
		: ECGroup::PLAYERS_ENEMIES
	};

	for (const EntityHandle_t ehandle : ec->getGroup(player_group))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

		if (!pl || pl->deadflag() || pl == local
			|| (cfg::aimbot_ignore_friends && pl->IsPlayerOnSteamFriendsList())
			|| (cfg::aimbot_ignore_invisible && pl->IsInvisible())
			|| (cfg::aimbot_ignore_invulnerable && pl->IsInvulnerable())) {
			continue;
		}

		const int target_hitbox_id{ HITBOX_BODY };

		for (size_t n{}; n < lag_comp->getNumRecords(pl); n++)
		{
			const LagRecord *const lr{ lag_comp->getRecord(pl, n) };

			if (!lr || lr->hitboxes.size() <= target_hitbox_id) {
				continue;
			}

			const vec3 lr_pos{ lr->hitboxes[target_hitbox_id] };
			const vec3 ang_to{ math::calcAngle(local->GetEyePos(), lr_pos) };
			const float fov_to{ math::calcFOV(i::engine->GetViewAngles(), ang_to) };
			const float dist_to{ local->GetEyePos().distTo(lr_pos) };

			if (cfg::aimbot_melee_mode == 0 && fov_to > cfg::aimbot_melee_fov) {
				continue;
			}

			targets.push_back({ pl, ang_to, lr_pos, fov_to, dist_to, lr, n == 0, false });
		}
	}

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::BUILDINGS_ENEMIES))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_BaseObject *const obj{ ent->cast<C_BaseObject>() };

		if (!obj) {
			continue;
		}

		const vec3 obj_pos{ obj->GetCenter() };
		const vec3 ang_to{ math::calcAngle(local->GetEyePos(), obj_pos) };
		const float fov_to{ math::calcFOV(i::engine->GetViewAngles(), ang_to) };
		const float dist_to{ local->GetEyePos().distTo(obj_pos) };

		if (cfg::aimbot_melee_mode == 0 && fov_to > cfg::aimbot_melee_fov) {
			continue;
		}

		targets.push_back({ obj, ang_to, obj_pos, fov_to, dist_to, nullptr, true, false });
	}

	if (targets.empty()) {
		return false;
	}

	std::sort(targets.begin(), targets.end(), [](const Target &a, const Target &b)
	{
		if (cfg::aimbot_melee_mode == 0) {
			return a.m_fov_to < b.m_fov_to;
		}

		if (cfg::aimbot_melee_mode == 1) {
			return a.m_dist_to < b.m_dist_to;
		}

		return false;
	});

	size_t swing_pred_attempts{};

	for (size_t n{}; n < targets.size(); n++)
	{
		Target &target{ targets[n] };

		//s: 6 should be plenty
		if (swing_pred_attempts >= 6 && target.m_can_swing_pred) {
			continue;
		}

		if (!processTarget(local, weapon, target))
		{
			if (target.m_can_swing_pred) {
				swing_pred_attempts++;
			}

			continue;
		}

		out = target;

		return true;
	}

	return false;
}

void AimbotMelee::setAttack(CUserCmd *const cmd, C_TFPlayer *const local, C_TFWeaponBase *const weapon, const Target &target)
{
	if (!cmd || !local || !weapon || !cfg::aimbot_auto_shoot || cfg::aimbot_mode == 1) {
		return;
	}

	if (cfg::aimbot_melee_aim_method == 1)
	{
		vec3 forward{};
		math::angleVectors(cmd->viewangles, &forward);

		float swing_range{};

		vec3 hull_mins{};
		vec3 hull_maxs{};

		if (!getMeleeInfo(local, weapon, swing_range, hull_mins, hull_maxs)) {
			return;
		}

		const vec3 trace_start{ local->GetEyePos() };
		const vec3 trace_end{ trace_start + (forward * swing_range) };

		trace_filters::melee_aim.m_ignore_ent = local;

		trace_t trace{};

		if (target.m_was_swing_preded)
		{
			const AimUtils::SetAbsOrigin set_abs_origin{ target.m_ent, target.m_pos };

			tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
		}

		else
		{
			if (const LagRecord *const lr{ target.m_lag_record }) {
				lrm->set(target.m_ent->cast<C_TFPlayer>(), lr);
				tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
				lrm->reset();
			}

			else {
				tf_utils::traceHull(trace_start, trace_end, hull_mins, hull_maxs, &trace_filters::melee_aim, &trace, MASK_SOLID);
			}
		}

		if (trace.m_pEnt == target.m_ent) {
			cmd->buttons |= IN_ATTACK;
		}
	}

	else {
		cmd->buttons |= IN_ATTACK;
	}
}

void AimbotMelee::setAngles(CUserCmd *const cmd, const Target &target)
{
	if (!cmd) {
		return;
	}

	const vec3 target_angle{ target.m_ang_to };

	if (cfg::aimbot_melee_aim_method == 0) {
		cmd->viewangles = target_angle;
		i::engine->SetViewAngles(cmd->viewangles);
	}

	if (cfg::aimbot_melee_aim_method == 1)
	{
		vec3 delta_ang{ target_angle - cmd->viewangles };

		math::clampAngles(delta_ang);

		const float smooth{ math::remap(cfg::aimbot_melee_smooth, 1.0f, 100.0f, 1.5f, 30.0f) };

		if (delta_ang.length() > 0.0f && smooth) {
			cmd->viewangles += delta_ang / smooth;
			i::engine->SetViewAngles(cmd->viewangles);
		}
	}

	if (cfg::aimbot_melee_aim_method == 2) {
		tf_utils::fixMovement(cmd, target_angle);
		cmd->viewangles = target_angle;
	}

	if (cfg::aimbot_melee_aim_method == 3)
	{
		if (aim_utils->isAttacking(cmd, i::global_vars->curtime)) {
			tf_utils::fixMovement(cmd, target_angle);
			cmd->viewangles = target_angle;
			tf_globals::send_packet = false;
		}
	}
}

void AimbotMelee::run(CUserCmd *const cmd, C_TFPlayer *const local, C_TFWeaponBase *const weapon)
{
	if (!cfg::aimbot_melee_active || !cmd || !local || !weapon) {
		return;
	}

	m_local_positions.clear();

	Target target{};

	if (!getTarget(local, weapon, target)) {
		return;
	}

	setAttack(cmd, local, weapon, target);
	setAngles(cmd, target);

	aim_utils->setTickCount(cmd, target.m_ent, target.m_lag_record);

	tf_globals::aimbot_target = target.m_ent ? target.m_ent->GetRefEHandle() : EntityHandle_t{};
}