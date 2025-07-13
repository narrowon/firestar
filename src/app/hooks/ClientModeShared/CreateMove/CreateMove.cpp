#include "../../../hooks.hpp"

MAKE_HOOK(
	ClientModeShared_CreateMove,
	s::ClientModeShared_CreateMove.get(),
	bool,
	void* rcx, float flInputSampleTime, CUserCmd* cmd)
{
	const bool result{ CALL_ORIGINAL(rcx, flInputSampleTime, cmd) };

	if (!cmd || !cmd->command_number) {
		return result;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	engine_pred->update();

	if (result) {
		i::engine->SetViewAngles(cmd->viewangles);
		i::pred->SetLocalViewAngles(cmd->viewangles);
	}

	cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;

	input_tick->processStart();

	//if we're warping we need to adjust our tickbase immediately to get all of our features to use the correct curtime
	if (tick_base->checkWarp(cmd))
	{
		const int host_frameticks{ *s::host_frameticks.cast<int*>() };
		const int ticks_to_shift{ std::min(tick_base->m_processing_ticks - i::client_state->chokedcommands, 21) - (host_frameticks - 1) };
		const int server_tick{ tf_utils::timeToTicks(tf_utils::getServerTime() + tf_utils::getLatency()) };
		const int adjusted_tick{ tick_base->getAdjustedTick(ticks_to_shift + 1, local->m_nTickBase(), server_tick) };

		local->m_nTickBase() = adjusted_tick;
	}

	misc->tauntSpin(cmd);
	misc->sentryWiggler(cmd);
	misc->bhop(cmd);
	misc->fastStop(cmd);
	misc->autostrafe(cmd);
	misc->noisemakerSpam();
	misc->mvmRespawn();
	misc->autoDisguise();

	engine_pred->edgeJump(cmd);

	tf_globals::aimbot_target = EntityHandle_t{};

	if (engine_pred->start(cmd))
	{
		aim_utils->fixAttackButtons(cmd);
		aimbot->run(cmd);
		auto_detonate->run(cmd);
		aim_utils->fixMinigunButtons(cmd);
		crits->run(cmd);

		engine_pred->end();
	}

	if (tick_base->canDoubleTap(cmd) || tick_base->m_should_warp) {
		tick_base->sendMoreMove(cmd);
	}

	input_tick->processEnd();

	tf_globals::last_cmd = *cmd;

	return false;
}