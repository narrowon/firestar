#include "../../../hooks.hpp"

MAKE_HOOK(
	CTFPlayerShared_InCond,
	s::CTFPlayerShared_InCond.get(),
	bool,
	void* rcx, ETFCond eCond)
{
	if (C_TFPlayer* const pl{ *reinterpret_cast<C_TFPlayer**>(reinterpret_cast<uintptr_t>(rcx) + 440 /*CTFPlayerShared::m_pOuter*/) })
	{
		if (cfg::remove_scope && pl->entindex() == i::engine->GetLocalPlayer())
		{
			const uintptr_t ret_address{ reinterpret_cast<uintptr_t>(_ReturnAddress()) };

			if (ret_address == s::CTFPlayerShared_InCond_CHudScope_ShouldDraw_Call.get()) {
				return false;
			}

			if (pl->InFirstPersonView())
			{
				if (ret_address == s::CTFPlayerShared_InCond_ClientModeTFNormal_ShouldDrawViewModel_Call.get()) {
					return false;
				}
			}

			else
			{
				if (ret_address == s::CTFPlayerShared_InCond_C_TFPlayer_ShouldDraw_Call.get()
					|| ret_address == s::CTFPlayerShared_InCond_C_TFWearable_ShouldDraw_Call.get())
				{
					return false;
				}
			}
		}

		if (eCond == TF_COND_DISGUISED)
		{
			if (C_TFPlayer* const local{ ec->getLocal() })
			{
				if (pl->m_iTeamNum() != local->m_iTeamNum()) {
					return false;
				}
			}
		}
	}

	return CALL_ORIGINAL(rcx, eCond);
}