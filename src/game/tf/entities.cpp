#include "entities.hpp"
#include "interfaces.hpp"

#include "../../app/features/crits/crits.hpp"

IHandleEntity *CBaseHandle::Get() const
{
    return i::ent_list->GetClientEntityFromHandle(m_Index);
}

C_TFWeaponBase *const C_BaseCombatCharacter::Weapon_OwnsThisID(const int weaponID)
{
	for (int n{}; n < MAX_WEAPONS; n++)
	{
		C_TFWeaponBase *const weapon{ GetWeapon(n) };

		if (!weapon) {
			continue;
		}

		if (weapon->GetWeaponID() == weaponID) {
			return weapon;
		}
	}

	return nullptr;
}

bool C_TFPlayer::IsCritBoosted()
{
	const bool bAllWeaponCritActive
	{
		InCond(TF_COND_CRITBOOSTED)
		|| InCond(TF_COND_CRITBOOSTED_PUMPKIN)
		|| InCond(TF_COND_CRITBOOSTED_USER_BUFF)
		|| InCond(TF_COND_CRITBOOSTED_FIRST_BLOOD)
		|| InCond(TF_COND_CRITBOOSTED_BONUS_TIME)
		|| InCond(TF_COND_CRITBOOSTED_CTF_CAPTURE)
		|| InCond(TF_COND_CRITBOOSTED_ON_KILL)
		|| InCond(TF_COND_CRITBOOSTED_CARD_EFFECT)
		|| InCond(TF_COND_CRITBOOSTED_RUNE_TEMP)
	};

	if (bAllWeaponCritActive) {
		return true;
	}

	if (C_TFWeaponBase *const pWeapon{ m_hActiveWeapon().Get()->cast<C_TFWeaponBase>() })
	{
		if (InCond(TF_COND_CRITBOOSTED_RAGE_BUFF) && pWeapon->m_pWeaponInfo()->m_iWeaponType == TF_WPN_TYPE_PRIMARY) {
			return true;
		}

		const float flCritHealthPercent{ tf_utils::attribHookValue(1.0f, "mult_crit_when_health_is_below_percent", pWeapon) };

		if (flCritHealthPercent < 1.0f && HealthFraction() < flCritHealthPercent) {
			return true;
		}
	}

	return false;
}