#include "../../../hooks.hpp"


MAKE_HOOK(
	C_BaseEntity_EstimateAbsVelocity,
	s::C_BaseEntity_EstimateAbsVelocity.get(),
	void,
	void* rcx, Vector& vel)
{
	//walkway
	/*if (rcx != ec->getLocal()) {
		vel = {};
		return;
	}*/

	if (C_BaseEntity* const ent{ reinterpret_cast<C_BaseEntity*>(rcx) })
	{
		if (ent->GetClassId() == class_ids::CTFPlayer) {
			vel = ent->cast<C_TFPlayer>()->m_vecVelocity();
			return;
		}
	}

	CALL_ORIGINAL(rcx, vel);
}