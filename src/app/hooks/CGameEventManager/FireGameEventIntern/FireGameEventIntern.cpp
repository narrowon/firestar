#include "../../../hooks.hpp"

MAKE_HOOK(
	CGameEventManager_FireGameEventIntern,
	s::CGameEventManager_FireGameEventIntern.get(),
	bool,
	void* rcx, IGameEvent* event, bool server_only, bool client_only)
{
	if (!client_only) {
		return CALL_ORIGINAL(rcx, event, server_only, client_only);
	}

	for (HasGameEvent* const h : getInsts<HasGameEvent>())
	{
		if (!h) {
			continue;
		}

		h->onGameEvent(event);
	}

	return CALL_ORIGINAL(rcx, event, server_only, client_only);
}