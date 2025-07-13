#include "../../../hooks.hpp"


MAKE_HOOK(
	ISurface_OnScreenSizeChanged,
	mem::findVirtual(i::surface, 111).get(),
	void,
	void* rcx, int nOldWidth, int nOldHeight)
{
	CALL_ORIGINAL(rcx, nOldWidth, nOldHeight);

	for (HasScreenSizeChange* const h : getInsts<HasScreenSizeChange>())
	{
		if (!h) {
			continue;
		}

		h->onScreenSizeChange();
	}
}