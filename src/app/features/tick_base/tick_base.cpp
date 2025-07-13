#include "tick_base.hpp"

#include <iostream>

#include "../engine_pred/engine_pred.hpp"
#include "../aim_utils/aim_utils.hpp"
#include "../visual_utils/visual_utils.hpp"
#include "../notifs/notifs.hpp"
#include "../cfg.hpp"
#include "../misc/misc.hpp"

//#define DEBUG_DT

#ifdef DEBUG_DT
MAKE_SIG(server_UTIL_EntityByIndex, mem::findBytes("server.dll", "E8 ? ? ? ? 8B 53 40 45 33 C0").fixRip());
#endif

bool TickBase::onLoad()
{
	m_processing_ticks = 0;
	m_tick_fixes.clear();

	return true;
}

bool TickBase::onLevelInit()
{
	return onLoad();
}

int TickBase::getAdjustedTick(const int sim_ticks, const int tickbase, const int server_tick)
{
	static ConVar* const sv_clockcorrection_msecs{ i::cvar->FindVar("sv_clockcorrection_msecs") };

	if (!sv_clockcorrection_msecs) {
		return 0;
	}

	if (sim_ticks < 0) return tickbase;

	if (i::global_vars->maxClients == 1) {
		return i::global_vars->tickcount - sim_ticks + 1;
	}

	float latency = tf_utils::getLatency();
	int latency_ticks = tf_utils::timeToTicks(latency);
	int correction_ticks = tf_utils::timeToTicks(std::clamp(sv_clockcorrection_msecs->GetFloat() / 1000.0f, 0.0f, 1.0f));

	int ideal_final_tick = server_tick + correction_ticks + latency_ticks;
	int estimated_final_tick = tickbase + sim_ticks;
	int fast_limit = ideal_final_tick + correction_ticks;

	return (estimated_final_tick > fast_limit || estimated_final_tick < server_tick) ? ideal_final_tick - sim_ticks + 1 : tickbase;
}

int TickBase::getShiftLimit(const bool warp)
{
	int host_frameticks = *s::host_frameticks.cast<int*>();
	int ticks_to_shift = std::min(m_processing_ticks - i::client_state->chokedcommands, 21) - (host_frameticks - 1);

	ticks_to_shift = std::min(ticks_to_shift, 16);
	return warp ? std::min(ticks_to_shift, 6) : ticks_to_shift;
}

void TickBase::sendMoreMove(CUserCmd* const cmd)
{
	if (!m_should_doubletap && !m_should_warp) return;

	auto* local = ec->getLocal();
	if (!local || local->deadflag()) return;

	int ticks_to_shift{};

	if (engine_pred->start(cmd)) {
		 m_should_antiwarp = shouldAntiWarp(); bool on_ground = (local->m_MoveType() == MOVETYPE_WALK) && (local->m_fFlags() & (1 << 0)); ticks_to_shift = on_ground ? std::min(22, m_processing_ticks - i::client_state->chokedcommands) : getShiftLimit(m_should_warp); m_shift_start_origin = local->m_vecOrigin() + (local->m_vecVelocity() * math::remap(local->m_vecVelocity().length2D(), 0.0f, 400.0f, 0.0f, 0.4f) * tf_utils::ticksToTime(2)); 
		 engine_pred->end();
	}

	int latest_cmd = engine_pred->getLatestCommandNumber();
	int new_outgoing_cmd = latest_cmd + 1;

	if (tf_globals::final_tick) tf_globals::send_packet = true;

	int server_tick = tf_utils::timeToTicks(tf_utils::getServerTime() + tf_utils::getLatency()) - 1;
	int adjusted_tick = getAdjustedTick(ticks_to_shift + 1, local->m_nTickBase(), server_tick);
	local->m_nTickBase() = adjusted_tick;
	addTickShift(latest_cmd, adjusted_tick);

	m_shift_start_origin = local->m_vecOrigin() + (local->m_vecVelocity() * math::remap(local->m_vecVelocity().length2D(), 0.0f, 400.0f, 0.0f, 0.4f) * tf_utils::ticksToTime(2));
	m_shift_velocity = local->m_vecVelocity();

	while (ticks_to_shift >= 1 && i::client_state->chokedcommands < 24) {
		bool final_tick = (ticks_to_shift == 1 || i::client_state->chokedcommands == 22) && tf_globals::final_tick;
		i::engine->GetNetChannelInfo()->SetChoked();
		i::client_state->chokedcommands++;
		engine_pred->update();

		addTickShift(new_outgoing_cmd, adjusted_tick + new_outgoing_cmd - latest_cmd);
		CUserCmd* new_cmd = &i::input->m_pCommands[new_outgoing_cmd % 90];
		std::memcpy(new_cmd, cmd, sizeof(CUserCmd));
		new_cmd->hasbeenpredicted = false;

		if (m_should_antiwarp) antiWarp(new_cmd);
		else {
			misc->bhop(new_cmd);
			misc->fastStop(new_cmd);
			engine_pred->edgeJump(new_cmd);
		}

		//anti_aim->run(new_cmd, final_tick);
		new_outgoing_cmd++;
		ticks_to_shift--;
	}

	m_disable_recharge = false;
	m_should_doubletap = false;
}

bool TickBase::rechargeTicks()
{
	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		m_recharge = false;
		return false;
	}

	if (m_processing_ticks < 24) {
		if (input_tick->getKeyInGame(cfg::tb_recharge_key).held && !m_recharge && !m_disable_recharge) {
			m_recharge = true;
		}

		if (m_processing_ticks <= 1) {
			m_disable_recharge = false;
		}

		if (m_recharge) {
			return true;
		}
	}
	else {
		m_recharge = false;
		m_disable_recharge = true; // stop users from recharging those bled ticks
	}

	return false;
}
bool TickBase::canDoubleTap(CUserCmd* const cmd)
{
	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return false;
	}
	C_TFWeaponBase* const wep{ ec->getWeapon() };

	if (!wep) {
		return false;
	}

	if (!cmd || m_processing_ticks < 8 || !input_tick->getKeyInGame(cfg::tb_dt_key).held) {
		return false;
	}

	const int ticks_to_shift{ getShiftLimit() };
	const int server_tick{ tf_utils::timeToTicks(tf_utils::getServerTime() + tf_utils::getLatency()) };

	if (const float adjusted_time{ tf_utils::ticksToTime(getAdjustedTick(ticks_to_shift + 2, local->m_nTickBase(), server_tick)) }; aim_utils->isAttacking(cmd, adjusted_time, true)) {
		m_should_doubletap = true;
		return true;
	}

	if (cfg::tb_dt_lock) {
		if (!tf_utils::isWeaponChargeable(wep)
			&& (wep->GetWeaponID() == TF_WEAPON_MINIGUN
				&& wep->cast<C_TFMinigun>()->m_iWeaponState() == AC_STATE_FIRING))
		{
			cmd->buttons &= ~IN_ATTACK;
		}
	}

	return false;
}

bool TickBase::checkWarp(CUserCmd* const cmd)
{
	m_should_warp = false;

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return false;
	}

	if (!cmd || m_processing_ticks <= 1) {
		return false;
	}

	if (!input_tick->getKeyInGame(cfg::tb_warp_key).held) {
		return false;
	}

	m_should_warp = true;
	return true;
}

void TickBase::updateTickbaseFixes()
{
	//dont store a crazy amount of tickbase fixes
	while (m_tick_fixes.size() > 24) {
		m_tick_fixes.pop_front();
	}

	for (auto iter{ m_tick_fixes.begin() }; iter != m_tick_fixes.end(); iter++)
	{
		if (i::client_state->last_command_ack >= (*iter).command_number) {
			m_tick_fixes.erase(iter);
			iter--;
		}
	}
}


void TickBase::antiWarp(CUserCmd* const cmd)
{
	if (!cmd || !m_should_antiwarp)
		return;

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local
		|| local->deadflag()
		|| local->m_MoveType() != MOVETYPE_WALK
		|| !(local->m_fFlags() & FL_ONGROUND))
	{
		return;
	}

	if (cfg::tb_antiwarp_style == 0)
	{
		cmd->forwardmove = cmd->sidemove = 0.f;
	}
	else if (cfg::tb_antiwarp_style == 1)
	{
		tf_utils::walkTo(cmd, local->m_vecOrigin(), m_shift_start_origin, 1.0f);
	}
}
bool TickBase::shouldAntiWarp() const
{
	if (m_should_warp && !m_should_doubletap) {
		return false;
	}

	C_TFWeaponBase* const wep{ ec->getWeapon() };

	if (!wep) {
		return false;
	}

	switch (cfg::tb_antiwarp_mode)
	{
	case 0:
		return false;
	case 1: {
		if (tf_utils::isWeaponProjectile(wep) || tf_utils::isWeaponMelee(wep)) {
			return false;
		}
		return true;
	}
	case 2:
		return true;
	default:
		return false;
	}
	return false;
}



bool TickBase::isAttacking(CUserCmd* const cmd, float curtime, const bool vis_indicator)
{
	if (!cmd) {
		return false;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return false;
	}

	C_TFWeaponBase* const weapon{ ec->getWeapon() };

	if (!weapon) {
		return false;
	}

	//adjust our curtime to our non-shifted tickbase if we have a charge weapon
	if (tf_utils::isWeaponChargeable(weapon)) {
		curtime = local->m_nTickBase() * i::global_vars->interval_per_tick;
	}

	const bool can_primary_attack{
		(curtime >= local->m_flNextAttack()) && (curtime >= weapon->m_flNextPrimaryAttack())
	};

	const float charge_begin_time{ weapon->cast<C_TFPipebombLauncher>()->m_flChargeBeginTime() };

	switch (weapon->GetWeaponID())
	{
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	{
		if (vis_indicator) {
			return can_primary_attack;
		}

		if (charge_begin_time)
		{
			const float charge{ curtime - charge_begin_time };
			const float max_charge{ tf_utils::attribHookValue(4.0f, "stickybomb_charge_rate", weapon) };

			if ((charge > max_charge) && (cmd->buttons & IN_ATTACK)) {
				return true;
			}
		}

		return (!(cmd->buttons & IN_ATTACK) && charge_begin_time);
	}

	case TF_WEAPON_COMPOUND_BOW:
	case TF_WEAPON_CANNON:
	case TF_WEAPON_SNIPERRIFLE_CLASSIC:
	{
		if (vis_indicator) {
			return can_primary_attack;
		}

		return (!(cmd->buttons & IN_ATTACK) && charge_begin_time);
	}

	default: {
		return aim_utils->isAttacking(cmd, curtime, true);
	}
	}

	return false;
}

void TickBase::addTickShift(const int command_number, const int tickbase)
{
	auto it = std::ranges::find_if(m_tick_fixes, [&](const TickShift& shift) { return shift.command_number == command_number; });
	if (it != m_tick_fixes.end()) it->tickbase = tickbase;
	else m_tick_fixes.emplace_back(command_number, tickbase);
}

void TickBase::paint()
{
	if (!cfg::tb_indicator || !vis_utils->shouldRunVisuals()) {
		return;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	static int ticks_to_lerp = 0;

	if (m_processing_ticks > 18) {
		ticks_to_lerp = 22;
	}
	else {
		ticks_to_lerp = m_processing_ticks;
	}

	static float pt_lerped = static_cast<float>(ticks_to_lerp);


	pt_lerped += (static_cast<float>(ticks_to_lerp) - pt_lerped) * (10.0f * i::global_vars->frametime);
	pt_lerped = std::clamp(pt_lerped, 0.0f, 22.0f);

	ImGui::SetNextWindowSize(ImVec2(130, 23), ImGuiCond_Always);
	ImGui::Begin("tickbase", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
	{
		ImVec2 bar_pos = ImGui::GetWindowPos();
		const float bar_width = 130.0f;
		const float bar_height = 23.0f;
		const float max_ticks = 22.0f;

		const float fill_ratio = std::clamp(pt_lerped / max_ticks, 0.0f, 1.0f);
		const float fill_width = bar_width * fill_ratio;

		ImColor bar_color;
		if (m_should_doubletap && m_processing_ticks >= 22) {
			bar_color = ImColor(204,204,204,255);
		}
		else if (m_recharge || m_processing_ticks < 22) {
			bar_color = ImColor(150, 150, 150, 255);
		}
		else {
			bar_color = ImColor(204, 204, 204, 255);
		}

		auto* draw_list = ImGui::GetWindowDrawList();

		draw_list->AddRectFilled(bar_pos, ImVec2(bar_pos.x + bar_width, bar_pos.y + bar_height), IM_COL32(20, 20, 20, 255));

		draw_list->AddRectFilled(bar_pos, ImVec2(bar_pos.x + fill_width, bar_pos.y + bar_height), bar_color);

		draw_list->AddRect(bar_pos, ImVec2(bar_pos.x + bar_width, bar_pos.y + bar_height), IM_COL32(255, 255, 255, 40));

	

		ImVec2 text_pos = ImVec2(bar_pos.x + bar_width / 2, bar_pos.y + bar_height / 2);
		draw_list->AddText(ImVec2(text_pos.x - ImGui::CalcTextSize("TICKBASE").x / 2, text_pos.y - ImGui::GetFontSize() / 2),
						   IM_COL32(180, 180, 180, 200), "TICKBASE");
		
	}
	ImGui::End();
}