#include "../../../hooks.hpp"

MAKE_HOOK(
	C_BasePlayer_PostDataUpdate,
	s::C_BasePlayer_PostDataUpdate.get(),
	void,
	void* rcx, DataUpdateType_t updateType)
{
	C_TFPlayer* const pl{ reinterpret_cast<C_TFPlayer*>(reinterpret_cast<uintptr_t>(rcx) - 0x10) };

	player_data->postDataUpdate(pl);

	CALL_ORIGINAL(rcx, updateType);

	anims->postDataUpdate(pl);
	lag_comp->addRecord(pl);
}