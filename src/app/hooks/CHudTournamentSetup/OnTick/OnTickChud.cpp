#include "../../../hooks.hpp"

MAKE_HOOK(
	CHudTournamentSetup_OnTick,
	s::CHudTournamentSetup_OnTick.get(),
	void,
	void* rcx)
{
	CALL_ORIGINAL(rcx);

	misc->autoMvmReadyUp();
}