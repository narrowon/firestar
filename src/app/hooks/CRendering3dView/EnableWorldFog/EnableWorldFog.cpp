#include "../../../hooks.hpp"

MAKE_HOOK(
	CRendering3dView_EnableWorldFog,
	s::CRendering3dView_EnableWorldFog.get(),
	void,
	void* rcx)
{
	if (cfg::remove_fog)
	{
		if (IMatRenderContext* const rc{ i::mat_sys->GetRenderContext() })
		{
			rc->FogMode(MATERIAL_FOG_LINEAR);
			rc->FogColor3f(0.0f, 0.0f, 0.0f);
			rc->FogStart(0.0f);
			rc->FogEnd(0.0f);
			rc->FogMaxDensity(0.0f);

			return;
		}
	}

	CALL_ORIGINAL(rcx);
}