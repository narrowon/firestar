#include "../../../hooks.hpp"

MAKE_HOOK(
	CUserMessages_DispatchUserMessage,
	s::CUserMessages_DispatchUserMessage.get(),
	bool,
	void* rcx, int msg_type, bf_read& msg_data)
{
	if (msg_type == 10 && cfg::remove_shake) {
		return true;
	}

	if (msg_type == 11 && cfg::remove_fade) {
		return true;
	}

	return CALL_ORIGINAL(rcx, msg_type, msg_data);
}