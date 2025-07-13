#include "../../../hooks.hpp"

MAKE_HOOK(
	C_TFPlayer_CreateMove,
	s::C_TFPlayer_CreateMove.get(),
	bool,
	void* rcx, float flInputSampleTime, CUserCmd* cmd)
{
	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return CALL_ORIGINAL(rcx, flInputSampleTime, cmd);
	}

	const bool taunting{ local->InCond(TF_COND_TAUNTING) };

	if (cfg::misc_taunts_slide && taunting && local->m_bAllowMoveDuringTaunt()) {
		return false;
	}

	if (cfg::misc_remove_teammate_pushaway && !taunting) {
		return false;
	}

	return CALL_ORIGINAL(rcx, flInputSampleTime, cmd);
}