#include "../../../hooks.hpp"


MAKE_HOOK(
	CTFPlayerShared_IsPlayerDominated,
	s::CTFPlayerShared_IsPlayerDominated.get(),
	bool,
	void* rcx, int index)
{
	const bool result{ CALL_ORIGINAL(rcx, index) };

	if (cfg::reveal_scoreboard
		&& !result
		&& reinterpret_cast<uintptr_t>(_ReturnAddress()) == s::CTFPlayerShared_IsPlayerDominated_RS_From.get())
	{
		*reinterpret_cast<uintptr_t*>(_AddressOfReturnAddress()) = s::CTFPlayerShared_IsPlayerDominated_RS_To.get();
	}

	return result;
}