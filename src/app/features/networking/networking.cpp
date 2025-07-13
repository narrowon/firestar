#include "networking.hpp"

#include "../engine_pred/engine_pred.hpp"
#include "../tick_base/tick_base.hpp"

void Networking::sendMove()
{
	byte data[4000]{};
	CLC_Move msg{};

	const int next_command_number{ engine_pred->getLatestCommandNumber() };

	msg.m_DataOut.StartWriting(data, sizeof(data));
	msg.m_nNewCommands = std::clamp(1 + i::client_state->chokedcommands, 0, MAX_NEW_COMMANDS);

	const int extra_commands{ i::client_state->chokedcommands + 1 - msg.m_nNewCommands };
	const int backup_commands{ std::max(2, extra_commands) };

	msg.m_nBackupCommands = std::clamp(backup_commands, 0, MAX_BACKUP_COMMANDS);

	const int num_cmds{ msg.m_nNewCommands + msg.m_nBackupCommands };

	int from{ -1 };
	bool ok{ true };

	for (int to{ next_command_number - num_cmds + 1 }; to <= next_command_number; to++) {
		const bool new_cmd{ to >= next_command_number - msg.m_nNewCommands + 1 };
		ok = ok && i::client->WriteUsercmdDeltaToBuffer(&msg.m_DataOut, from, to, new_cmd);
		from = to;
	}

	if (ok)
	{
		if (extra_commands > 0) {
			//whoever the fuck did this i hate u
			*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(i::engine->GetNetChannelInfo()) + 32) -= extra_commands;
		}

		s::CNetChan_SendNetMsg.call<bool>(i::client_state->m_NetChannel, &msg, false, false);

		tick_base->m_processing_ticks -= i::client_state->chokedcommands + 1;
		tick_base->m_processing_ticks = std::max(0, tick_base->m_processing_ticks);
	}
}

void Networking::move(float accumulated_extra_samples, bool final_tick)
{
	static ConVar* host_limitlocal{ i::cvar->FindVar("host_limitlocal") };

	const double net_time{ *s::net_time.cast<double*>() };

	if (!i::client_state->IsConnected() || !s::Host_ShouldRun.call<bool>()) {
		return;
	}

	tf_globals::send_packet = true;
	tf_globals::final_tick = final_tick;

	if (i::demo_player->IsPlayingBack())
	{
		if (i::client_state->ishltv || i::client_state->isreplay) {
			tf_globals::send_packet = false;
		}

		else {
			return;
		}
	}

	if ((!i::client_state->m_NetChannel->IsLoopback() || host_limitlocal->GetInt()) &&
		((net_time < i::client_state->m_flNextCmdTime) || !i::client_state->m_NetChannel->CanPacket() || !final_tick))
	{
		tf_globals::send_packet = false;
	}

	if (i::client_state->IsActive())
	{
		const int next_command_number{ engine_pred->getLatestCommandNumber() };

		const int host_frameticks{ *s::host_frameticks.cast<int*>() };
		const int host_currentframetick{ *s::host_currentframetick.cast<int*>() };

		// client tick loss can happen from the game not running frames due to the engine_no_sleep_focus cvar
		// blocking out this cvar will minimalize the loss but will still happen unfortunately
		// bitch ass hoes that actually have this set to anything but 0 can eat my ass
		static ConVar* const engine_no_focus_sleep{ i::cvar->FindVar("engine_no_focus_sleep") };

		if (engine_no_focus_sleep->GetInt() != 0)
			engine_no_focus_sleep->SetValue(0);

		//if multiple cl_move calls in 1 frame, inc only on the first one
		if (host_frameticks == 1 || host_currentframetick == 1) {
			tick_base->m_processing_ticks++;
		}
		tick_base->m_processing_ticks = std::min(24, tick_base->m_processing_ticks);

		//tickbase fixing after recharging ticks is very trivial and unneeded
		if (tick_base->rechargeTicks()) {
			goto RECHARGE;
		}

		i::client->CreateMove(next_command_number, i::global_vars->interval_per_tick - accumulated_extra_samples, !i::client_state->IsPaused());

		if (i::demo_recorder->IsRecording()) {
			i::demo_recorder->RecordUserInput(next_command_number);
		}

		if (tf_globals::send_packet) {
			sendMove();
		}

		else {
			i::client_state->m_NetChannel->SetChoked();
			i::client_state->chokedcommands++;
		}
	}

RECHARGE:

	if (!tf_globals::send_packet) {
		return;
	}

	const bool has_problem{ i::client_state->m_NetChannel->IsTimingOut() && !i::demo_player->IsPlayingBack() && i::client_state->IsActive() };

	if (has_problem) {
		//mfed: game draws the timing out message here:
		i::client_state->m_nDeltaTick = -1;
	}

	if (i::client_state->IsActive())
	{
		const float host_frametime_unbounded{ *s::host_frametime_unbounded.cast<float*>() };
		const float host_frametime_stddeviation{ *s::host_frametime_stddeviation.cast<float*>() };

		const NET_Tick mymsg(i::client_state->m_nDeltaTick, host_frametime_unbounded, host_frametime_stddeviation);

		s::CNetChan_SendNetMsg.call<bool>(i::client_state->m_NetChannel, &mymsg, false, false);
	}

	i::client_state->lastoutgoingcommand = s::CNetChan_SendDatagram.call<int>(i::client_state->m_NetChannel, nullptr);
	i::client_state->chokedcommands = 0;

	if (i::client_state->IsActive())
	{
		static ConVar* cl_cmdrate{ i::cvar->FindVar("cl_cmdrate") };

		const float command_interval{ 1.0f / cl_cmdrate->GetFloat() };
		const float max_delta{ std::min(i::global_vars->interval_per_tick, command_interval) };
		const float delta{ std::clamp((float)net_time - (float)i::client_state->m_flNextCmdTime, 0.0f, max_delta) };

		i::client_state->m_flNextCmdTime = net_time + command_interval - delta;
#ifdef OPTIMAL_CMD_RATE
		i::client_state->m_flNextCmdTime = net_time + i::global_vars->interval_per_tick;
#endif
	}

	else {
		i::client_state->m_flNextCmdTime = net_time + 0.2f;
	}
}

bool Networking::shouldReadPackets()
{
	if (!i::engine->IsInGame()) {
		return true;
	}

	INetChannelInfo* const net_channel{ i::engine->GetNetChannelInfo() };

	if (!net_channel || net_channel->IsLoopback()) {
		return true;
	}

	read_packets_state->restore();

	return false;
}

void Networking::fixNetworking(bool final_tick)
{
	if (!i::engine->IsInGame()) {
		return;
	}

	INetChannelInfo* const net_channel{ i::engine->GetNetChannelInfo() };

	if (!net_channel || net_channel->IsLoopback()) {
		return;
	}

	read_packets_state_backup->store();

	static Hook* const read_packets = [&]() -> Hook*
		{
			for (Hook* const hook : getInsts<Hook>())
			{
				if (hook->name().compare("CL_ReadPackets") != 0) {
					continue;
				}

				return hook;
			}

			return nullptr;
		}();

	read_packets->call<void(__fastcall*)(bool)>()(final_tick);

	read_packets_state->store();
	read_packets_state_backup->restore();
}