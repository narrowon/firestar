#pragma once

#include "class_ids.hpp"

namespace tf_utils
{
	int timeToTicks(const float time);
	float ticksToTime(const int tick);
	bool isEntOwnedBy(C_BaseEntity *const ent, C_BaseEntity *const who);
	void trace(const vec3 &start, const vec3 &end, ITraceFilter *const filter, trace_t *const tr, const int mask);
	void traceHull(const vec3 &start, const vec3 &end, const vec3 &mins, const vec3 &maxs, ITraceFilter *const filter, trace_t *const tr, const int mask);
	bool getHitbox(C_BaseAnimating* const animating, const matrix3x4_t* const matrix, const int hitbox, vec3& out);
	bool getHitbox(C_BaseAnimating *const animating, const int hitbox, vec3 &out);
	int getHitboxCount(C_BaseAnimating *const animating);
	float getLerp();
	float getLatency();
	void fixMovement(CUserCmd *const cmd, const Vector &target_angle);
	bool tauntMovesForward(const TauntIndex_t taunt);
	void walkTo(CUserCmd *const cmd, const vec3 &from, const vec3 &to, const float speed);
	bool isWeaponHitscan(C_TFWeaponBase *const wep);
	bool isWeaponMelee(C_TFWeaponBase *const wep);
	bool isWeaponProjectile(C_TFWeaponBase *const wep);
	bool isWeaponChargeable(C_TFWeaponBase *const wep);
	float getGravity();
	void getProjectileFireSetup(const vec3 &angs, vec3 offset, vec3 &pos_out);
	float getServerTime();
	float randomFloat(const float min, const float max);
	int randomInt(const int min, const int max);
	void randomSeed(const unsigned int seed);
	bool isBehindAndFacing(const vec3 &angle, C_TFPlayer *const local, C_TFPlayer *const other);
}