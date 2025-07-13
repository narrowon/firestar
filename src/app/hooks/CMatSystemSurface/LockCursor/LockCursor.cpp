#include "../../../hooks.hpp"

MAKE_HOOK(
	CMatSystemSurface_LockCursor,
	s::CMatSystemSurface_LockCursor.get(),
	void,
	void* rcx)
{
	if (menu->isOpen()) {
		i::surface->UnlockCursor();
		return;
	}

	CALL_ORIGINAL(rcx);
}