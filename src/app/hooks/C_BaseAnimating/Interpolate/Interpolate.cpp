#include "../../../hooks.hpp"


MAKE_HOOK(
	C_BaseAnimating_Interpolate,
	s::C_BaseAnimating_Interpolate.get(),
	bool,
	void* rcx, float flCurrentTime)
{
	if (C_BaseAnimating* const animating{ reinterpret_cast<C_BaseAnimating*>(rcx) })
	{
		if (animating->m_bPredictable())
		{
			if (tick_base->m_recharge)
				return true;
		}
		else
		{
			if (cfg::remove_interp)
				return true;
		}
	}

	return CALL_ORIGINAL(rcx, flCurrentTime);
}