#include "../../../hooks.hpp"

MAKE_HOOK(
	C_TFPlayer_ThirdPersonSwitch,
	s::C_TFPlayer_ThirdPersonSwitch.get(),
	void,
	void* rcx)
{
	CALL_ORIGINAL(rcx);

	C_TFPlayer* const local{ ec->getLocal() };

	if (!rcx || !local || rcx != local) {
		return;
	}

	C_BaseEntity* const wep{ local->m_hActiveWeapon().Get() };

	if (!wep || wep->cast<C_TFWeaponBase>()->GetWeaponID() != TF_WEAPON_MINIGUN) {
		return;
	}

	C_TFMinigun* const minigun{ wep->cast<C_TFMinigun>() };

	if (!minigun || minigun->m_iState() != AC_STATE_FIRING) {
		return;
	}

	minigun->StartMuzzleEffect();
	minigun->StartBrassEffect();
}