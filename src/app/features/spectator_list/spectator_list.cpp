#include "spectator_list.hpp"

#include "../visual_utils/visual_utils.hpp"
#include "../cfg.hpp"

void SpectatorList::updateSpectators()
{
	m_spectators.clear();

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	C_TFPlayer *target{ local->deadflag() ? local->m_hObserverTarget().Get()->cast<C_TFPlayer>() : local };

	if (!target) {
		m_target_index = i::engine->GetLocalPlayer();
		return;
	}

	C_TFPlayerResource *const res{ ec->getPlayerResource() };

	m_target_index = target->entindex();

	for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PLAYERS_ALL))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

		if (!pl->deadflag()) {
			continue;
		}

		C_BaseEntity *const obs_target{ pl->m_hObserverTarget().Get() };

		if (!obs_target || obs_target->entindex() != m_target_index) {
			continue;
		}

		EObserverModes mode{ pl->m_iObserverMode() };

		// in_eye = 1st person, chase = 3rd person
		if (mode != OBS_MODE_IN_EYE && mode != OBS_MODE_CHASE) {
			continue;
		}

		player_info_t info{};

		if (!i::engine->GetPlayerInfo(pl->entindex(), &info)) {
			continue;
		}

		m_spectators.emplace_back(
			info.name,
			res ? res->GetNextRespawnTime(pl->entindex()) : 0.0f,
			mode,
			vis_utils->getEntColor(pl)
		);
	}
}

void SpectatorList::run()
{
	if (!cfg::spectator_list_active || !vis_utils->shouldRunVisuals()) {
		return;
	}

	updateSpectators();

	if (m_spectators.empty()) {
		return;
	}

	player_info_t info{};
	if (!i::engine->GetPlayerInfo(m_target_index, &info)) {
		return;
	}

	// Persistent window position


	ImGui::SetNextWindowPos(ImVec2(50, 100), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 25), ImGuiCond_Once);

	if (ImGui::Begin("Spectator List", nullptr,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();

		const ImVec2 screen_size = ImGui::GetIO().DisplaySize;
		ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
		float height = fonts::font_esp->FontSize + 2.0f;

		const std::string name = (m_target_index == i::engine->GetLocalPlayer()) ? "you" : info.name;
		const std::string header = std::format("spectating {}", name);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		cursor_pos.y += height;
		for (const Spectator& spec : m_spectators) {
			const std::string spec_name = spec.toString();
			const Color& color = spec.getColor();

			draw_list->AddText(fonts::font_esp, fonts::font_esp->FontSize, cursor_pos,
				IM_COL32(color.r, color.g, color.b, color.a), spec_name.c_str());

			cursor_pos.y += height;
		}
	}
	ImGui::End();

}

const std::string SpectatorList::Spectator::toString() const
{
	const std::string observer_mode{ m_mode == OBS_MODE_IN_EYE ? "1st" : "3rd" };
	const float displayed_respawn_time{ fabsf(i::global_vars->curtime - m_respawn_time) };

	if (cfg::spectator_list_show_respawn_time && m_respawn_time > 0.0f && displayed_respawn_time < 2000.0f) { // mfed: fix for saxton hale mode where the respawn time is like 200k
		return std::format("[{}] {} ({:.1f}s)", observer_mode, m_name, displayed_respawn_time);
	}

	return std::format("[{}] {}", observer_mode, m_name);
}

const Color &SpectatorList::Spectator::getColor() const
{
	if (cfg::spectator_list_override_firstperson && m_mode == OBS_MODE_IN_EYE) {
		return cfg::spectator_list_firstperson_clr;
	}

	return m_color;
}