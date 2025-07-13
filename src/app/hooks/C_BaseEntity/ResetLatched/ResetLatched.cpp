#include "../../../hooks.hpp"

MAKE_HOOK(
	C_BaseEntity_ResetLatched,
	s::C_BaseEntity_ResetLatched.get(),
	void,
	void* rcx)
{
	if (C_BaseEntity* const ent{ reinterpret_cast<C_BaseEntity*>(rcx) })
	{
		if (ent->entindex() == i::engine->GetLocalPlayer()) {
			return;
		}
	}

	CALL_ORIGINAL(rcx);
}