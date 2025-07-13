#include "../../../hooks.hpp"

MAKE_HOOK(
	CPrediction_RunSimulation,
	s::CPrediction_RunSimulation.get(),
	void,
	CPrediction* rcx, int current_command, float curtime, CUserCmd* cmd, C_BasePlayer* player)
{
	if (tick_base->m_recharge) {
		return;
	}

	engine_pred->current_prediction_cmd = current_command;

	for (const auto& tick_shifts : tick_base->m_tick_fixes)
	{
		// only place we can do tickbase fix since our command numbers will get screwed up by crithack and we currently dont
		// restore them after we send the cmd off to the server
		if (tick_shifts.command_number == current_command) {
			player->m_nTickBase() = tick_shifts.tickbase;
			curtime = tf_utils::ticksToTime(player->m_nTickBase());
			break;
		}
	}

	CALL_ORIGINAL(rcx, current_command, curtime, cmd, player);
}