#include "../../../hooks.hpp"

MAKE_HOOK(
	CMatchInviteNotification_OnTick,
	s::CMatchInviteNotification_OnTick.get(),
	void,
	void* rcx)
{
	if (cfg::misc_remove_autojoin_countdown)
	{
		const uintptr_t notification{ reinterpret_cast<uintptr_t>(rcx) };
		const int32_t offset{ *s::CMatchInviteNotification_Flags_Offset.cast<int32_t*>() };
		const uint32_t flags{ *reinterpret_cast<uint32_t*>(notification + offset) };

		if ((flags & ~2)) {
			return;
		}
	}

	CALL_ORIGINAL(rcx);
}