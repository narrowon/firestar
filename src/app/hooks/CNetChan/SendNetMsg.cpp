#include "../../hooks.hpp"

MAKE_HOOK(
	CNetChan_SendNetMsg,
	s::CNetChan_SendNetMsg.get(),
	bool,
	CNetChan* rcx, INetMessage* msg, bool reliable, bool voice)
{
	if (msg->GetGroup() == INetChannelInfo::VOICE) {
		voice = true;
	}

	if (msg->GetType() == clc_FileCRCCheck)
	{
		if (cfg::misc_sv_pure_bypass) {
			return true;
		}
	}

	if (msg->GetType() == clc_setNetConvar) {
		if (cfg::interp_abuse) {
			auto* netData = static_cast<NET_SetConVar*>(msg);
			if (strcmp(netData->m_ConVars.name, "cl_interpolate") == 0) {
				snprintf(netData->m_ConVars.value, sizeof(netData->m_ConVars.value), "%f", cfg::interp_amount);
			}
			if (strcmp(netData->m_ConVars.name, "cl_interp_ratio") == 0) {
				snprintf(netData->m_ConVars.value, sizeof(netData->m_ConVars.value), "%f", cfg::interp_ratio_amount);
			}
		}
		return true; // is this required?
	}
	return CALL_ORIGINAL(rcx, msg, reliable, voice);
}