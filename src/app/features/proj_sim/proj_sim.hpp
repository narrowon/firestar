#pragma once

#include "../../../game/game.hpp"

class ProjSim final
{
private:
	struct ProjectileInfo final
	{
		ProjectileType_t m_type{};
		vec3 m_pos{};
		vec3 m_ang{};
		float m_speed{};
		float m_gravity_mod{};
		bool m_no_spin{};
	};

private:
	IPhysicsEnvironment *m_phys_env{};
	IPhysicsObject *m_phys_obj{};

private:
	bool getProjInfo(C_TFPlayer *const pl, C_TFWeaponBase *const wep, const vec3 &angs, ProjectileInfo &out);
	void projFireSetup(C_TFPlayer *const pl, vec3 offset, const vec3 &ang_in, vec3 &pos_out, vec3 &ang_out, bool pipes);

public:
	bool init(C_TFPlayer *const pl, C_TFWeaponBase *const wep, const vec3 &angs);
	void tick();
	vec3 origin();
};

MAKE_UNIQUE(ProjSim, proj_sim);