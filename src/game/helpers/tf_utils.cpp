#include "tf_utils.hpp"

int tf_utils::timeToTicks(const float time)
{
	return static_cast<int>(0.5f + time / i::global_vars->interval_per_tick);
}

float tf_utils::ticksToTime(const int tick)
{
	return i::global_vars->interval_per_tick * static_cast<float>(tick);
}

bool tf_utils::isEntOwnedBy(C_BaseEntity *const ent, C_BaseEntity *const who)
{
	if (!ent || !who) {
		return false;
	}

	const size_t classId{ ent->GetClassId() };

	if (classId == class_ids::CTFGrenadePipebombProjectile
		|| classId == class_ids::CTFProjectile_Jar
		|| classId == class_ids::CTFProjectile_JarGas
		|| classId == class_ids::CTFProjectile_JarMilk
		|| classId == class_ids::CTFProjectile_Cleaver)
	{
		return ent->cast<C_BaseGrenade>()->m_hThrower().Get() == who;
	}

	if (classId == class_ids::CTFProjectile_SentryRocket)
	{
		if (C_BaseEntity *const owner{ ent->m_hOwnerEntity().Get() }) {
			return owner->cast<C_BaseObject>()->m_hBuilder() == who;
		}
	}

	if (classId == class_ids::CObjectSentrygun
		|| classId == class_ids::CObjectDispenser
		|| classId == class_ids::CObjectTeleporter)
	{
		return ent->cast<C_BaseObject>()->m_hBuilder() == who;
	}

	return ent->m_hOwnerEntity().Get() == who;
}

void tf_utils::trace(const vec3 &start, const vec3 &end, ITraceFilter *const filter, trace_t *const tr, const int mask)
{
	Ray_t ray{};

	ray.Init(start, end);

	i::trace->TraceRay(ray, mask, filter, tr);
}

void tf_utils::traceHull(const vec3 &start, const vec3 &end, const vec3 &mins, const vec3 &maxs, ITraceFilter *const filter, trace_t *const tr, const int mask)
{
	Ray_t ray{};

	ray.Init(start, end, mins, maxs);

	i::trace->TraceRay(ray, mask, filter, tr);
}

bool tf_utils::getHitbox(C_BaseAnimating* const animating, const matrix3x4_t* const matrix, const int hitbox, vec3& out)
{
	if (!animating || !matrix) {
		return false;
	}

	const model_t* const model{ animating->GetModel() };

	if (!model) {
		return false;
	}

	studiohdr_t* const studio_model{ i::model_info->GetStudioModel(model) };

	if (!studio_model) {
		return false;
	}

	mstudiobbox_t* const bbox{ studio_model->pHitbox(hitbox, animating->m_nHitboxSet()) };

	if (!bbox) {
		return false;
	}

	math::vectorTransform((bbox->bbmin + bbox->bbmax) * 0.5f, matrix[bbox->bone], out);

	return true;
}

bool tf_utils::getHitbox(C_BaseAnimating *const animating, const int hitbox, vec3 &out)
{
	if (!animating) {
		return false;
	}

	const model_t *const model{ animating->GetModel() };

	if (!model) {
		return false;
	}

	studiohdr_t *const studio_model{ i::model_info->GetStudioModel(model) };

	if (!studio_model) {
		return false;
	}

	mstudiobbox_t *const bbox{ studio_model->pHitbox(hitbox, animating->m_nHitboxSet()) };

	if (!bbox) {
		return false;
	}

	matrix3x4_t bone_matrix[128]{};

	if (!animating->SetupBones(bone_matrix, 128, BONE_USED_BY_ANYTHING, i::global_vars->curtime)) {
		return false;
	}

	math::vectorTransform((bbox->bbmin + bbox->bbmax) * 0.5f, bone_matrix[bbox->bone], out);

	return true;
}

int tf_utils::getHitboxCount(C_BaseAnimating *const animating)
{
	if (!animating) {
		return 0;
	}

	const model_t *const model{ animating->GetModel() };

	if (!model) {
		return 0;
	}

	studiohdr_t *const studio_model{ i::model_info->GetStudioModel(model) };

	if (!studio_model) {
		return 0;
	}

	return studio_model->iHitboxCount(animating->m_nHitboxSet());
}

float tf_utils::getLerp()
{
	static ConVar *cl_interp{ i::cvar->FindVar("cl_interp") };
	static ConVar *cl_interp_ratio{ i::cvar->FindVar("cl_interp_ratio") };
	static ConVar *cl_updaterate{ i::cvar->FindVar("cl_updaterate") };

	return std::max(cl_interp->m_fValue, cl_interp_ratio->m_fValue / cl_updaterate->m_fValue);
}

float tf_utils::getLatency()
{
	INetChannelInfo *const net{ i::engine->GetNetChannelInfo() };

	if (!net) {
		return 0.0f;
	}

	return net->GetLatency(FLOW_INCOMING) + net->GetLatency(FLOW_OUTGOING);;
}

void tf_utils::fixMovement(CUserCmd *const cmd, const Vector &target_angle)
{
	Vector move_vec{ cmd->forwardmove, cmd->sidemove, cmd->upmove };
	Vector move_direction{};

	math::vectorAngles(move_vec, move_direction);

	const float speed{ sqrtf(move_vec.x * move_vec.x + move_vec.y * move_vec.y) };
	const float yaw{ math::degToRad(target_angle.y - cmd->viewangles.y + move_direction.y) };

	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;
}

bool tf_utils::tauntMovesForward(const TauntIndex_t taunt)
{
	if (taunt == INVALID_ITEM_DEF_INDEX) {
		return false;
	}

	// mfed: i have a items_game.txt parser to get these, its an attribute. will have to find it if they add more taunts in the future
	switch (taunt)
	{
		case Taunt_Conga:
		case Taunt_Panzer_Pants:
		case Taunt_The_Scooty_Scoot:
		case Taunt_Scotsmanns_Stagger:
		case Taunt_The_Skating_Scorcher:
		case Taunt_The_Bunnyhopper:
		case Taunt_Rocket_Jockey:
		case Taunt_The_Boston_Boarder:
		case Taunt_Texas_Truckin:
		case Taunt_The_Mannbulance:
		case Taunt_The_Hot_Wheeler:
		case Taunt_Drunk_Manns_Cannon:
		case Taunt_Shanty_Shipmate:
		case Taunt_The_Road_Rager: {
			return true;
		}

		default: {
			break;
		}
	}

	return false;
}

void tf_utils::walkTo(CUserCmd *const cmd, const vec3 &from, const vec3 &to, const float speed)
{
	const vec3 difference{ to - from };

	if (difference.length() == 0.0f) {
		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;
		return;
	}

	vec3 direction{};
	math::vectorAngles({ difference.x, difference.y, 0.0f }, direction);

	const float yaw{ math::degToRad(direction.y - cmd->viewangles.y) };

	constexpr float move_speed{ 450.0f };

	cmd->forwardmove = cosf(yaw) * (speed * move_speed);
	cmd->sidemove = -sinf(yaw) * (speed * move_speed);
}

bool tf_utils::isWeaponHitscan(C_TFWeaponBase *const wep)
{
	if (!wep || isWeaponProjectile(wep) || isWeaponMelee(wep)) {
		return false;
	}

	return (wep->GetDamageType() & DMG_BULLET) || (wep->GetDamageType() & DMG_BUCKSHOT);
}

bool tf_utils::isWeaponMelee(C_TFWeaponBase *const wep)
{
	if (!wep) {
		return false;
	}

	return wep->GetSlot() == 2;
}

bool tf_utils::isWeaponProjectile(C_TFWeaponBase *const wep)
{
	if (!wep) {
		return false;
	}

	switch (wep->GetWeaponID())
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		{
			if (wep->m_iItemDefinitionIndex() == Soldier_m_RocketJumper) {
				return false;
			}

			return true;
		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			if (wep->m_iItemDefinitionIndex() == Demoman_s_StickyJumper) {
				return false;
			}

			return true;
		}

		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_FLAREGUN:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_DRG_POMSON:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_FLAREGUN_REVENGE:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_FLAME_BALL:
		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE: {
			return true;
		}

		default: {
			return false;
		}
	}
}

bool tf_utils::isWeaponChargeable(C_TFWeaponBase* const wep)
{
	if (!wep) {
		return false;
	}

	switch (wep->GetWeaponID())
	{
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC: {
			return true;
		}

		default: {
			return false;
		}
	}
}

float tf_utils::getGravity()
{
	static ConVar *const sv_gravity{ i::cvar->FindVar("sv_gravity") };

	if (!sv_gravity) {
		return 0.0f;
	}

	return sv_gravity->GetFloat();
}

void tf_utils::getProjectileFireSetup(const vec3 &angs, vec3 offset, vec3 &pos_out)
{
	static ConVar *cl_flipviewmodels{ i::cvar->FindVar("cl_flipviewmodels") };

	if (!cl_flipviewmodels) {
		return;
	}

	if (cl_flipviewmodels->GetInt()) {
		offset.y *= -1.0f;
	}

	vec3 forward{};
	vec3 right{};
	vec3 up{};

	math::angleVectors(angs, &forward, &right, &up);

	pos_out += (forward * offset.x) + (right * offset.y) + (up * offset.z);
}

float tf_utils::getServerTime()
{
	return ticksToTime(i::client_state->m_ClockDriftMgr.m_nServerTick + 1) /*+ i::client_state->m_NetChannel->GetLatency(FLOW_OUTGOING)*/;
}

float tf_utils::randomFloat(const float min, const float max)
{
	static const MemAddr addr{ mem::findExport("vstdlib.dll", "RandomFloat") };

	if (!addr) {
		return 0.0f;
	}

	return addr.call<float, float>(min, max);
}

int tf_utils::randomInt(const int min, const int max)
{
	static const MemAddr addr{ mem::findExport("vstdlib.dll", "RandomInt") };

	if (!addr) {
		return 0;
	}

	return addr.call<int, int>(min, max);
}

void tf_utils::randomSeed(const unsigned int seed)
{
	static const MemAddr addr{ mem::findExport("vstdlib.dll", "RandomSeed") };

	if (!addr) {
		return;
	}

	addr.call<unsigned int>(seed);
}

bool tf_utils::isBehindAndFacing(const vec3 &angle, C_TFPlayer *const local, C_TFPlayer *const other)
{
	if (!local || !other) {
		return false;
	}

	const vec3 local_center{ local->GetCenter() };
	const vec3 other_center{ other->GetCenter() };

	vec3 to_target{};
	to_target = other_center - local_center;
	to_target.z = 0.0f;
	to_target.normalize();

	vec3 owner_forward{};
	math::angleVectors(angle, &owner_forward, nullptr, nullptr);
	owner_forward.z = 0.0f;
	owner_forward.normalize();

	vec3 target_forward{};
	math::angleVectors(other->m_angEyeAngles(), &target_forward, nullptr, nullptr);
	target_forward.z = 0.0f;
	target_forward.normalize();

	return (to_target.dot(target_forward) > 0.1f) && (to_target.dot(owner_forward) > 0.5f) && (target_forward.dot(owner_forward) > -0.2f);
}