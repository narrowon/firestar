#include "../../../hooks.hpp"

MAKE_HOOK(
	CMatSystemSurface_SetCursor,
	s::CMatSystemSurface_SetCursor.get(),
	void,
	void* rcx, HCursor cursor)
{
	if (menu->isOpen()) {
		cursor = dc_arrow;
	}

	CALL_ORIGINAL(rcx, cursor);
}