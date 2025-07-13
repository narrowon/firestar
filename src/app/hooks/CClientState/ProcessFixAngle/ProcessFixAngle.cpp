#include "../../../hooks.hpp"

MAKE_HOOK(
	CClientState_ProcessFixAngle,
	s::CClientState_ProcessFixAngle.get(),
	bool,
	void* rcx, void* msg)
{
	if (cfg::misc_remove_fix_angle) {
		return false;
	}

	return CALL_ORIGINAL(rcx, msg);
}