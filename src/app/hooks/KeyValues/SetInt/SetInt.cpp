#include "../../../hooks.hpp"


MAKE_HOOK(
	KeyValues_SetInt,
	s::KeyValues_SetInt.get(),
	void,
	void* rcx, const char* keyName, int value)
{
	CALL_ORIGINAL(rcx, keyName, value);

	if (cfg::reveal_scoreboard
		&& reinterpret_cast<uintptr_t>(_ReturnAddress()) == s::KeyValues_SetInt_RS_From.get()
		&& !strcmp(keyName, "nemesis"))
	{
		*reinterpret_cast<uintptr_t*>(_AddressOfReturnAddress()) = s::KeyValues_SetInt_RS_To.get();
	}
}
