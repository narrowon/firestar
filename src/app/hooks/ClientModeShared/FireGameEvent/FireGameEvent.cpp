#include "../../../hooks.hpp"

MAKE_HOOK(
	ClientModeShared_FireGameEvent,
	s::ClientModeShared_FireGameEvent.get(),
	void,
	void* rcx, IGameEvent* event)
{
	CALL_ORIGINAL(rcx, event);
}