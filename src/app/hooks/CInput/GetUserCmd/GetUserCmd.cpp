#include "../../../hooks.hpp"


MAKE_HOOK(
	CInput_GetUserCmd, s::CInput_GetUserCmd.get(),
	CUserCmd*,
	void* rcx, int sequence_number)
{
	return &i::input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}
