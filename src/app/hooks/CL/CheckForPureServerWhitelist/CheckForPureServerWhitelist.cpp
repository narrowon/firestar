#include "../../../hooks.hpp"

MAKE_HOOK(
	CL_CheckForPureServerWhitelist,
	s::CL_CheckForPureServerWhitelist.get(),
	void,
	void** pFilesToReload)
{
	if (cfg::misc_sv_pure_bypass) {
		return;
	}

	CALL_ORIGINAL(pFilesToReload);
}
