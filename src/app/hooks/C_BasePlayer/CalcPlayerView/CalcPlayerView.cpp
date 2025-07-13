#include "../../../hooks.hpp"

MAKE_HOOK(
	C_BasePlayer_CalcPlayerView,
	s::C_BasePlayer_CalcPlayerView.get(),
	void,
	void* rcx, Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
{
	if (cfg::remove_punch)
	{
		if (C_BasePlayer* const pl{ reinterpret_cast<C_BasePlayer*>(rcx) })
		{
			const vec3 original_punch_angle{ pl->m_vecPunchAngle() };

			pl->m_vecPunchAngle() = {};
			CALL_ORIGINAL(rcx, eyeOrigin, eyeAngles, fov);
			pl->m_vecPunchAngle() = original_punch_angle;

			return;
		}
	}

	CALL_ORIGINAL(rcx, eyeOrigin, eyeAngles, fov);
}