#include "../../../hooks.hpp"

MAKE_HOOK(
	CHLClient_LevelShutdown,
	s::CHLClient_LevelShutdown.get(),
	void,
	void* rcx)
{
	CALL_ORIGINAL(rcx);

	for (HasLevelShutdown* const h : getInsts<HasLevelShutdown>())
	{
		if (!h) {
			continue;
		}

		h->onLevelShutdown();
	}
}