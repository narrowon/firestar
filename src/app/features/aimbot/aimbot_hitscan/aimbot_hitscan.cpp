	#include "aimbot_hitscan.hpp"

#include "../../aim_utils/aim_utils.hpp"
#include "../../trace_utils/trace_utils.hpp"
#include "../../cfg.hpp"

bool AimbotHitscan::weaponHasHeadshot(C_TFPlayer *const local, C_TFWeaponBase *const weapon)
{
	switch (weapon->GetWeaponID())
	{
		case TF_WEAPON_SNIPERRIFLE:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC:
		case TF_WEAPON_SNIPERRIFLE_DECAP:
		{
			const int rifle_type{ weapon->cast<C_TFSniperRifle>()->GetRifleType() };

			if (rifle_type == RIFLE_CLASSIC) {
				return true;
			}

			return rifle_type != RIFLE_JARATE && local->InCond(TF_COND_ZOOMED);
		}

		case TF_WEAPON_REVOLVER: {
			return weapon->cast<C_TFRevolver>()->CanHeadshot();
		}

		default: {
			return false;
		}
	}
}

int AimbotHitscan::getTargetHitbox(C_TFPlayer *const local, C_TFWeaponBase *const weapon)
{
	if (!local || !weapon) {
		return 0;
	}

	if (cfg::aimbot_hitscan_aim_pos == 0)
	{
		if (local->IsCritBoosted()) {
			return HITBOX_PELVIS;
		}

		if (weaponHasHeadshot(local, weapon)) {
			return HITBOX_HEAD;
		}

		return HITBOX_PELVIS;
	}

	if (cfg::aimbot_hitscan_aim_pos == 1) {
		return HITBOX_PELVIS;
	}

	if (cfg::aimbot_hitscan_aim_pos == 2) {
		return HITBOX_HEAD;
	}

	return 0;
}

bool AimbotHitscan::canSee(C_TFPlayer *const local, const Target &target)
{
	if (!local || !target.m_ent) {
		return false;
	}

	trace_filters::hitscan_aim.m_ignore_ent = local;

	trace_t trace{};

	if (const LagRecord *const lr{ target.m_lag_record }) {
		lrm->set(target.m_ent->cast<C_TFPlayer>(), lr);
		tf_utils::trace(local->GetEyePos(), target.m_pos, &trace_filters::hitscan_aim, &trace, MASK_SHOT | CONTENTS_GRATE);
		lrm->reset();
	}

	else {
		tf_utils::trace(local->GetEyePos(), target.m_pos, &trace_filters::hitscan_aim, &trace, MASK_SHOT | CONTENTS_GRATE);
	}

	return (trace.m_pEnt == target.m_ent || trace.fraction > 0.99f) && !trace.allsolid && !trace.startsolid;
}

bool AimbotHitscan::scanBBox(C_TFPlayer *const local, Target &target)
{
	if (!local || !target.m_ent) {
		return false;
	}

	for (float point_scale{ 0.9f }; point_scale > 0.5f; point_scale -= 0.1f)
	{
		const vec3 scaled_mins{ target.m_ent->m_vecMins() * point_scale };
		const vec3 scaled_maxs{ target.m_ent->m_vecMaxs() * point_scale };

		const std::vector<vec3> offsets
		{
			{ (scaled_mins.x + scaled_maxs.x) * 0.5f, (scaled_mins.y + scaled_maxs.y) * 0.5f, scaled_mins.z },
			{ (scaled_mins.x + scaled_maxs.x) * 0.5f, (scaled_mins.y + scaled_maxs.y) * 0.5f, scaled_maxs.z },

			{ scaled_mins.x, scaled_mins.y, (scaled_mins.z + scaled_maxs.z) * 0.5f  },
			{ scaled_maxs.x, scaled_mins.y, (scaled_mins.z + scaled_maxs.z) * 0.5f  },
			{ scaled_mins.x, scaled_maxs.y, (scaled_mins.z + scaled_maxs.z) * 0.5f  },
			{ scaled_maxs.x, scaled_maxs.y, (scaled_mins.z + scaled_maxs.z) * 0.5f  },

			{ scaled_mins.x, scaled_mins.y, scaled_maxs.z },
			{ scaled_maxs.x, scaled_mins.y, scaled_maxs.z },
			{ scaled_mins.x, scaled_maxs.y, scaled_maxs.z },
			{ scaled_maxs.x, scaled_maxs.y, scaled_maxs.z },

			{ scaled_mins.x, scaled_mins.y, scaled_mins.z },
			{ scaled_maxs.x, scaled_mins.y, scaled_mins.z },
			{ scaled_mins.x, scaled_maxs.y, scaled_mins.z },
			{ scaled_maxs.x, scaled_maxs.y, scaled_mins.z },
		};

		for (size_t n{}; n < offsets.size(); n++)
		{
			const vec3 point{ target.m_ent->m_vecOrigin() + offsets[n] };

			target.m_ang_to = math::calcAngle(local->GetEyePos(), point);
			target.m_pos = point;

			if (canSee(local, target)) {
				return true;
			}
		}
	}

	return false;
}

bool AimbotHitscan::processTarget(C_TFPlayer *const local, C_TFWeaponBase *const weapon, Target &target)
{
	if (!local || !weapon || !target.m_ent) {
		return false;
	}

	if (canSee(local, target)) {
		return true;
	}

	if (cfg::aimbot_hitscan_bbox_multipoint)
	{
		const size_t class_id{ target.m_ent->GetClassId() };

		if (class_id == class_ids::CObjectSentrygun
			|| class_id == class_ids::CObjectDispenser
			|| class_id == class_ids::CObjectTeleporter) {
			return scanBBox(local, target);
		}
	}

	return false;
}
const std::vector<int> HITBOXES_TO_CHECK = {
		HITBOX_HEAD,
	HITBOX_PELVIS,
		HITBOX_CHEST,
		HITBOX_THORAX,
		HITBOX_UPPER_CHEST
};

bool AimbotHitscan::getTarget(C_TFPlayer* const local, C_TFWeaponBase* const weapon, Target& out)
{
	if (!local || !weapon) {
		return false;
	}
	struct PrioritizedTarget {
		Target target;
		float priority; // Higher priority for pelvis
	};

	std::vector<PrioritizedTarget> prioritized_targets{};

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PLAYERS_ENEMIES))
	{
		IClientEntity* const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };
		if (!ent) {
			continue;
		}

		C_TFPlayer* const pl{ ent->cast<C_TFPlayer>() };
		if (!pl || pl->deadflag() ||
			(cfg::aimbot_ignore_friends && pl->IsPlayerOnSteamFriendsList()) ||
			(cfg::aimbot_ignore_invisible && pl->IsInvisible()) ||
			(cfg::aimbot_ignore_invulnerable && pl->IsInvulnerable())) {
			continue;
		}

		// Determine which hitboxes to check based on config
		std::vector<int> hitboxes_to_check;
		if (cfg::aimbot_hitscan_aim_pos == 0) {
			hitboxes_to_check = HITBOXES_TO_CHECK;
		}
		else {
			hitboxes_to_check.push_back(getTargetHitbox(local, weapon));
		}

		for (size_t n{}; n < lag_comp->getNumRecords(pl); n++)
		{
			const LagRecord* const lr{ lag_comp->getRecord(pl, n) };
			if (!lr) {
				continue;
			}

			// Iterate through the selected hitboxes
			for (int hitbox_id : hitboxes_to_check)
			{
				if (lr->hitboxes.size() <= static_cast<size_t>(hitbox_id)) {
					continue;
				}

				const vec3 lr_pos{ lr->hitboxes[hitbox_id] };
				const vec3 ang_to{ math::calcAngle(local->GetEyePos(), lr_pos) };
				const float fov_to{ math::calcFOV(i::engine->GetViewAngles(), ang_to) };
				const float dist_to{ local->GetEyePos().distTo(lr_pos) };

				if (cfg::aimbot_hitscan_mode == 0 && fov_to > cfg::aimbot_hitscan_fov) {
					continue;
				}

				// Assign priority: pelvis gets highest priority
				float priority = (hitbox_id == HITBOX_PELVIS) ? 1000.0f : 0.0f;

				Target target{ pl, ang_to, lr_pos, fov_to, dist_to, lr };
				prioritized_targets.push_back({ target, priority });
			}
		}
	}

	// Handle buildings and projectiles (unchanged, no hitbox priority needed)
	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::BUILDINGS_ENEMIES))
	{
		IClientEntity* const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };
		if (!ent) {
			continue;
		}

		C_BaseObject* const obj{ ent->cast<C_BaseObject>() };
		if (!obj) {
			continue;
		}

		const vec3 obj_pos{ obj->GetCenter() };
		const vec3 ang_to{ math::calcAngle(local->GetEyePos(), obj_pos) };
		const float fov_to{ math::calcFOV(i::engine->GetViewAngles(), ang_to) };
		const float dist_to{ local->GetEyePos().distTo(obj_pos) };

		if (cfg::aimbot_hitscan_mode == 0 && fov_to > cfg::aimbot_hitscan_fov) {
			continue;
		}

		Target target{ obj, ang_to, obj_pos, fov_to, dist_to, nullptr };
		prioritized_targets.push_back({ target, -1.0f }); // Negative priority for non-player targets
	}

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PROJECTILES_ENEMIES))
	{
		IClientEntity* const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };
		if (!ent) {
			continue;
		}

		C_TFGrenadePipebombProjectile* const obj{ ent->cast<C_TFGrenadePipebombProjectile>() };
		if (!obj || !cfg::aimbot_hitscan_target_stickies || !obj->m_bTouched()) {
			continue;
		}

		const vec3 obj_pos{ obj->GetCenter() };
		const vec3 ang_to{ math::calcAngle(local->GetEyePos(), obj_pos) };
		const float fov_to{ math::calcFOV(i::engine->GetViewAngles(), ang_to) };
		const float dist_to{ local->GetEyePos().distTo(obj_pos) };

		if (cfg::aimbot_hitscan_mode == 0 && fov_to > cfg::aimbot_hitscan_fov) {
			continue;
		}

		Target target{ obj, ang_to, obj_pos, fov_to, dist_to, nullptr };
		prioritized_targets.push_back({ target, -1.0f }); // Negative priority for non-player targets
	}

	if (prioritized_targets.empty()) {
		return false;
	}

	// Sort targets with priority as the primary criterion
	std::sort(prioritized_targets.begin(), prioritized_targets.end(), [](const PrioritizedTarget& a, const PrioritizedTarget& b)
		{
			if (a.priority != b.priority) {
				return a.priority > b.priority; 
			}

			// If priorities are equal, fall back to the original sorting (FOV or distance)
			if (cfg::aimbot_hitscan_mode == 0) {
				return a.target.m_fov_to < b.target.m_fov_to;
			}
			if (cfg::aimbot_hitscan_mode == 1) {
				return a.target.m_dist_to < b.target.m_dist_to;
			}
			return false;
		});

	// Process the sorted targets
	for (const PrioritizedTarget& pt : prioritized_targets)
	{
		Target target = pt.target;
		if (!processTarget(local, weapon, target)) {
			continue;
		}
		out = target;
		return true;
	}

	return false;
}

void AimbotHitscan::setAttack(CUserCmd *const cmd, C_TFPlayer *const local, C_TFWeaponBase *const weapon, const Target &target)
{
	if (!cmd || !local || !weapon || !weapon->HasPrimaryAmmoForShot() || !cfg::aimbot_auto_shoot || cfg::aimbot_mode == 1) {
		return;
	}

	if (cfg::aimbot_hitscan_wait_for_headshot && weaponHasHeadshot(local, weapon) && !local->IsCritBoosted())
	{
		switch (weapon->GetWeaponID())
		{
			case TF_WEAPON_SNIPERRIFLE:
			case TF_WEAPON_SNIPERRIFLE_CLASSIC:
			case TF_WEAPON_SNIPERRIFLE_DECAP:
			{
				if (static_cast<int>(tf_utils::attribHookValue(0.0f, "sniper_no_headshot_without_full_charge", weapon)) != 0)
				{
					if (!weapon->cast<C_TFSniperRifle>()->IsFullyCharged()) {
						cmd->buttons |= IN_ATTACK;
						return;
					}
				}

				if (static_cast<int>(tf_utils::attribHookValue(0.0f, "sniper_crit_no_scope", weapon)) == 0)
				{
					if ((tf_utils::ticksToTime(local->m_nTickBase()) - local->m_flFOVTime()) < TF_WEAPON_SNIPERRIFLE_NO_CRIT_AFTER_ZOOM_TIME) {
						return;
					}
				}

				break;
			}

			case TF_WEAPON_REVOLVER:
			{
				if (weapon->cast<C_TFRevolver>()->GetWeaponSpread() > 0.0f) {
					return;
				}

				break;
			}

			default: {
				break;
			}
		}
	}

	if (cfg::aimbot_hitscan_aim_method == 1)
	{
		vec3 forward{};
		math::angleVectors(cmd->viewangles, &forward);

		const vec3 trace_start{ local->GetEyePos() };
		const vec3 trace_end{ trace_start + (forward * 8192.0f) };

		trace_filters::hitscan_aim.m_ignore_ent = local;

		trace_t trace{};

		if (const LagRecord *const lr{ target.m_lag_record }) {
			lrm->set(target.m_ent->cast<C_TFPlayer>(), lr);
			tf_utils::trace(trace_start, trace_end, &trace_filters::hitscan_aim, &trace, MASK_SHOT | CONTENTS_GRATE);
			lrm->reset();
		}

		else {
			tf_utils::trace(trace_start, trace_end, &trace_filters::hitscan_aim, &trace, MASK_SHOT | CONTENTS_GRATE);
		}

		if (trace.m_pEnt == target.m_ent && !trace.allsolid && !trace.startsolid) {
			cmd->buttons |= IN_ATTACK;
		}
	}

	else {
		cmd->buttons |= IN_ATTACK;
	}

	if (weapon->GetWeaponID() == TF_WEAPON_SNIPERRIFLE_CLASSIC && weapon->cast<C_TFSniperRifleClassic>()->m_bCharging()) {
		cmd->buttons &= ~IN_ATTACK;
	}
}

void AimbotHitscan::setAngles(CUserCmd *const cmd, const Target &target)
{
	if (!cmd) {
		return;
	}

	const vec3 target_angle{ target.m_ang_to };

	if (cfg::aimbot_hitscan_aim_method == 0) {
		cmd->viewangles = target_angle;
		i::engine->SetViewAngles(cmd->viewangles);
	}

	if (cfg::aimbot_hitscan_aim_method == 1)
	{
		vec3 delta_ang{ target_angle - cmd->viewangles };

		math::clampAngles(delta_ang);

		const float smooth{ math::remap(cfg::aimbot_hitscan_smooth, 1.0f, 100.0f, 1.5f, 30.0f) };

		if (delta_ang.length() > 0.0f && smooth) {
			cmd->viewangles += delta_ang / smooth;
			i::engine->SetViewAngles(cmd->viewangles);
		}
	}

	if (cfg::aimbot_hitscan_aim_method == 2) {
		tf_utils::fixMovement(cmd, target_angle);
		cmd->viewangles = target_angle;
	}

	if (cfg::aimbot_hitscan_aim_method == 3)
	{
		if (aim_utils->isAttacking(cmd, i::global_vars->curtime)) {
			tf_utils::fixMovement(cmd, target_angle);
			cmd->viewangles = target_angle;
			//tf_globals::send_packet = false;
		}
	}
}

bool AimbotHitscan::passesHitChance(C_TFPlayer *const local, C_TFWeaponBase *const weapon, const Target &target)
{
	CTFWeaponInfo *const info{ weapon->m_pWeaponInfo() };

	if (!info) {
		return false;
	}

	const float time_since_last_shot{ i::global_vars->curtime - weapon->m_flLastFireTime() };

	bool perfect_shot{};

	int bullets_per_shot{ info->GetWeaponData(0).m_nBulletsPerShot };

	if (bullets_per_shot >= 1) {
		bullets_per_shot = tf_utils::attribHookValue(bullets_per_shot, "mult_bullets_per_shot", weapon);
	}

	else {
		bullets_per_shot = 1;
	}

	if (bullets_per_shot > 1 && time_since_last_shot > 0.25f)
	{
		perfect_shot = true;

		if (weapon->GetWeaponID() == TF_WEAPON_MINIGUN) {
			return true;
		}
	}

	else if (bullets_per_shot == 1 && time_since_last_shot > 1.25f) {
		return true;
	}

	Ray_t ray{};
	trace_t trace;

	int needed_hits{ static_cast<int>(std::ceilf((cfg::aimbot_hitscan_hitchance * 255.0f) / 100.0f)) };
	int total_hits{};

	if (perfect_shot) {
		total_hits += 1;
	}

	const float spread{ weapon->GetWeaponSpread() };
	const vec3 eye_pos{ local->GetEyePos() };

	for (int n{}; n < 255; n++)
	{
		tf_utils::randomSeed(n + 1);

		const float random_x{ tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f) };
		const float random_y{ tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f) };

		vec3 spread_vec{ random_x * spread, random_y * spread, 0.0f };

		vec3 forward{};
		vec3 right{};
		vec3 up{};

		math::angleVectors(target.m_ang_to, &forward, &right, &up);

		spread_vec = forward + (right * spread_vec.x) + (up * spread_vec.y);
		spread_vec.normalize();

		ray.Init(eye_pos, { eye_pos + spread_vec * 8192.0f });

		i::trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, target.m_ent, &trace);

		if (trace.m_pEnt == target.m_ent) {
			total_hits++;
		}

		if (total_hits >= needed_hits) {
			return true;
		}

		if ((255 - n + total_hits) < needed_hits) {
			return false;
		}
	}

	return false;
}

void AimbotHitscan::run(CUserCmd *const cmd, C_TFPlayer *const local, C_TFWeaponBase *const weapon)
{
	if (!cfg::aimbot_hitscan_active || !cmd || !local || !weapon) {
		return;
	}

	Target target{};

	if (!getTarget(local, weapon, target)) {
		return;
	}

	setAttack(cmd, local, weapon, target);
	setAngles(cmd, target);

	aim_utils->setTickCount(cmd, target.m_ent, target.m_lag_record);

	tf_globals::aimbot_target = target.m_ent ? target.m_ent->GetRefEHandle() : EntityHandle_t{};
}