#include "../../../hooks.hpp"


MAKE_HOOK(C_BasePlayer_PostThink,
	s::C_BasePlayer_PostThink.get(),
	void,
	C_BasePlayer* rcx)
{
	if (engine_pred->isInEnginePred()) {
		return;
	}

	return CALL_ORIGINAL(rcx);
}