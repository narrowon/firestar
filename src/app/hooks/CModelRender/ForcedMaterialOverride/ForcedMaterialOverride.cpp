#include "../../../hooks.hpp"

MAKE_HOOK(
	CModelRender_ForcedMaterialOverride,
	s::CModelRender_ForcedMaterialOverride.get(),
	void,
	void* rcx, IMaterial* mat, OverrideType_t type)
{
	if (tf_globals::block_mat_override) {
		return;
	}

	CALL_ORIGINAL(rcx, mat, type);
}