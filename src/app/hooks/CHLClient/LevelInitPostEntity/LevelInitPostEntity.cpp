#include "../../../hooks.hpp"

MAKE_HOOK(
	CHLClient_LevelInitPostEntity,
	s::CHLClient_LevelInitPostEntity.get(),
	void,
	void* rcx)
{
	CALL_ORIGINAL(rcx);

	for (HasLevelInit* const h : getInsts<HasLevelInit>())
	{
		if (!h) {
			continue;
		}

		h->onLevelInit();
	}
}