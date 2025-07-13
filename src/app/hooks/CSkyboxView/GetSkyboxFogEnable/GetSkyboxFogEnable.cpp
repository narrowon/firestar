#include "../../../hooks.hpp"


MAKE_HOOK(
	CSkyboxView_GetSkyboxFogEnable,
	s::CSkyboxView_GetSkyboxFogEnable.get(),
	bool,
	void* rcx)
{
	if (cfg::remove_sky_fog) {
		return false;
	}

	return CALL_ORIGINAL(rcx);
}