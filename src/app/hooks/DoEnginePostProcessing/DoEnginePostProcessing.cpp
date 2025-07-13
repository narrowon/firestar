#include "../../hooks.hpp"

MAKE_HOOK(
	DoEnginePostProcessing,
	s::DoEnginePostProcessing.get(),
	void,
	int x, int y, int w, int h, bool bFlashlightIsOn, bool bPostVGui)
{
	if (cfg::remove_post_processing) {
		return;
	}

	CALL_ORIGINAL(x, y, w, h, bFlashlightIsOn, bPostVGui);
}