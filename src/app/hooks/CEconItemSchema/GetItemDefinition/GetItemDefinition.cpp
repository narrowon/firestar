#include "../../../hooks.hpp"

MAKE_HOOK(
	CEconItemSchema_GetItemDefinition,
	s::CEconItemSchema_GetItemDefinition.get(),
	void*,
	void* rcx, int iItemIndex)
{
	void* const result{ CALL_ORIGINAL(rcx, iItemIndex) };
	const size_t offset = 524; // mfed: todo: sig this offset

	if (cfg::misc_remove_equip_regions && result)
	{
		// setting m_unEquipRegionMask and m_unEquipRegionConflictMask to 0
		*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(result) + offset) = 0;
		*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(result) + offset + 4) = 0;
	}

	return result;
}