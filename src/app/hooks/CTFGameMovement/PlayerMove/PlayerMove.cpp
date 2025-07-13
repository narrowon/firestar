#include "../../../hooks.hpp"

MAKE_HOOK(
	CTFGameMovement_PlayerMove,
	s::CTFGameMovement_PlayerMove.get(),
	void,
	void* rcx)
{
	if (move_sim->isRunning()) {
		return s::CGameMovement_PlayerMove.call<void>(rcx);
	}

	CALL_ORIGINAL(rcx);
}