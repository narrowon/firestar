#include "../../../hooks.hpp"


MAKE_HOOK(
	CParticleSystemMgr_DrawRenderCache,
	s::CParticleSystemMgr_DrawRenderCache.get(),
	void,
	void* rcx, bool bShadowDepth)
{
	if (tf_globals::in_CBaseWorldView_DrawExecute)
	{
		if (IMatRenderContext* const rc{ i::mat_sys->GetRenderContext() }) {
			rc->ClearBuffers(false, false, true);
		}

		outlines->run();
		lag_comp->visual();
	}

	CALL_ORIGINAL(rcx, bShadowDepth);
}