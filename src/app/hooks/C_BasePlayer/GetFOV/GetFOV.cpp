#include "../../../hooks.hpp"

MAKE_HOOK(
	C_BasePlayer_GetFOV,
	s::C_BasePlayer_GetFOV.get(),
	float,
	void* rcx)
{
	if (cfg::fov_override_active)
	{
		if (C_TFPlayer* const pl{ reinterpret_cast<C_TFPlayer*>(rcx) })
		{
			static ConVar* const fov_desired{ i::cvar->FindVar("fov_desired") };

			if (fov_desired)
			{
				const int original_fov{ pl->m_iFOV() };
				const int original_fov_start{ pl->m_iFOVStart() };

				pl->m_iFOV() = pl->InCond(TF_COND_ZOOMED) ? cfg::fov_override_value_zoomed : cfg::fov_override_value;

				if (original_fov_start == std::max(75, fov_desired->GetInt())) {
					pl->m_iFOVStart() = cfg::fov_override_value;
				}

				if (original_fov_start == TF_WEAPON_ZOOM_FOV) {
					pl->m_iFOVStart() = cfg::fov_override_value_zoomed;
				}

				const float result{ CALL_ORIGINAL(rcx) };

				pl->m_iFOV() = original_fov;
				pl->m_iFOVStart() = original_fov_start;

				return result;
			}
		}
	}

	return CALL_ORIGINAL(rcx);
}