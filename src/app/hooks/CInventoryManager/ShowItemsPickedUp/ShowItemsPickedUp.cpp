#include "../../../hooks.hpp"

MAKE_HOOK(
	CInventoryManager_ShowItemsPickedUp,
	s::CInventoryManager_ShowItemsPickedUp.get(),
	bool,
	void* rcx, bool bForce, bool bReturnToGame, bool bNoPanel)
{
	if (cfg::misc_remove_item_found_notification) {
		CALL_ORIGINAL(rcx, true, true, true);
		return false;
	}

	return CALL_ORIGINAL(rcx, bForce, bReturnToGame, bNoPanel);
}