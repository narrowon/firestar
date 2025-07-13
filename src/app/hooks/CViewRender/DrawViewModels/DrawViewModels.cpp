#include "../../../hooks.hpp"

MAKE_HOOK(
	CViewRender_DrawViewModels,
	s::CViewRender_DrawViewModels.get(),
	void,
	void* rcx, const CViewSetup& viewRender, bool drawViewmodel)
{
	CALL_ORIGINAL(rcx, viewRender, drawViewmodel);

	outlines->drawEffect();
}