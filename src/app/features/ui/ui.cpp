#include "ui.hpp"
#include "widgets.hpp"
#include "../cfg.hpp"

static void Styling() {
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.03f, 0.03f, 0.03f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 0;
	style.ChildRounding = 0;
	style.FrameRounding = 0;
	style.PopupRounding = 0;
	style.ScrollbarRounding = 0;
	style.GrabRounding = 0;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 0;
}

void Menu::run() {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		m_open = !m_open;

	using namespace ImGui;
	auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 550, 450 };
	ImGui::SetNextWindowSize(ImVec2(gs));
	SetNextWindowPos({ (float)i::client->GetScreenWidth() / 2 - 274, (float)i::client->GetScreenHeight() / 2 - 225 }, ImGuiCond_Once);
	Styling();
	auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
	if (m_open) {
		Begin("pipeline", nullptr, flags);
		{

			s = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			p = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			auto list = ImGui::GetWindowDrawList();
		
			ImGui::PushFont(fonts::font_menu);
			//Text("pipe            ");
			draw->call_string(list, fonts::font_menu, vec2(p.x, p.y), "pipe", Color(255, 255, 255, 255), 0);
			draw->call_string(list, fonts::font_menu, vec2(p.x + ImGui::CalcTextSize("pipeline").x - 15.f, p.y), "line", Color(255, 170, 173, 255), 0);
			//SetCursorPosX(ImGui::CalcTextSize("pipe  line").x - 15.f);
			//TextColored(, "line v3 - %s    ", g_Globals.logged_in); // this is really ugly AAAAAAAAAA
			

			if (menuTab("aimbot", tab == 0)) {
				tab = 0;
			}
			SameLine();
			if (menuTab("visuals", tab == 1)) {
				tab = 1;
			}
			SameLine();
			if (menuTab("hvh", tab == 2)) {
				tab = 2;
			}
			SameLine();
			if (menuTab("misc", tab == 3)) {
				tab = 3;
			}
			Separator();
			{
				if (tab == 0) {
					if (menuSubTab("general", sub == 0)) {
						sub = 0;
					}
					SameLine();
					if (menuSubTab("hitscan", sub == 1)) {
						sub = 1;
					}
					SameLine();
					if (menuSubTab("projectile", sub == 2)) {
						sub = 2;
					}
					SameLine();
					if (menuSubTab("melee", sub == 3)) {
						sub = 3;
					}
					// -- AIMBOT
					{
						if (sub == 0) {
							//SameLine();
							if (BeginTable("AimbotGeneralTable", 3)) {
								TableNextColumn();
								TableNextColumn();
								{
									//SameLine();
									if (groupBox("general")) {
										toggle("enabled", &cfg::aimbot_active);
										hotkey("key", &cfg::aimbot_key, &cfg::aimbot_key_style);
										toggle("auto shoot", &cfg::aimbot_auto_shoot);
										selectSingle("mode", &cfg::aimbot_mode, { {"on key", 0}, {"on attack", 1} });
									
									}
									endGroupBox();
								}
								TableNextColumn();
								{
									if (groupBox("misc")) {
										selectMulti("ignore", { {"friends", &cfg::aimbot_ignore_friends}, {"cloacked", &cfg::aimbot_ignore_invisible}, {"invunl", &cfg::aimbot_ignore_invulnerable} });
									}
									endGroupBox();
								}

								EndTable();
							}
						
						}
						if (sub == 1) {
							//SameLine();
							if (BeginTable("AimbotHitscanTable", 3)) { // ima continue tomorrow
								TableNextColumn();
								TableNextColumn();
								{
									//SameLine();
									if (groupBox("general")) {
										toggle("enabled", &cfg::aimbot_hitscan_active);
										selectSingle("aim type", &cfg::aimbot_hitscan_aim_method, { {"plain", 0}, {"smooth", 1}, {"silent", 2}, {"psilent", 3} });
										selectSingle("priority hitbox", &cfg::aimbot_hitscan_aim_pos, { {"auto", 0}, {"body", 1}, {"head", 2} });
										sliderFloat("fov", &cfg::aimbot_hitscan_fov, 1, 180, "%.1f");
										toggle("multipoint", &cfg::aimbot_hitscan_bbox_multipoint);
										toggle("wait for headshot", &cfg::aimbot_hitscan_wait_for_headshot);
										sliderFloat("mininum hitchance", &cfg::aimbot_hitscan_hitchance, 0.f, 100.f, "%.0f");
									}
									endGroupBox();
								}
								EndTable();
							}
						}
						if (sub == 2) {
							if (BeginTable("AimbotProjectileTable", 3)) {
								TableNextColumn();
								{
									if (groupBox("general")) {
										toggle("enabled", &cfg::aimbot_projectile_active);
										selectSingle("aim type", &cfg::aimbot_projectile_aim_method, { {"plain", 0}, {"silent", 1}, {"psilent", 2} });
										selectSingle("priority hitbox", &cfg::aimbot_projectile_aim_pos, { {"auto", 0}, {"feet", 1}, {"body", 2}, {"head", 3} });
										selectSingle("mode", &cfg::aimbot_projectile_mode, { {"fov to current", 0}, {"fov to pred", 1}, {"distance", 2}});
										sliderFloat("fov", &cfg::aimbot_projectile_fov, 1, 180, "%.1f");
										toggle("multipoint", &cfg::aimbot_projectile_bbox_multipoint);
										endGroupBox();
									}
								}
								TableNextColumn();
								{
									if (groupBox("visual")) {
										toggle("pred path", &cfg::prediction_path_enable);
										sliderFloat("pred duration", &cfg::prediction_path_duration, 0.1f, 5.f);
										colorPicker("pred color", &cfg::prediction_path_color);
										endGroupBox();
									}
								}

								TableNextColumn();
								{
									if (groupBox("misc")) {
										toggle("strafe pred", &cfg::aimbot_strafe_pred);
										selectSingle("splashbot", &cfg::aimbot_projectile_splash_multipoint, { {"off", 0}, {"on", 1}, {"preferred", 2}});
										sliderFloat("sim time", &cfg::aimbot_projectile_max_sim_time, 0.1f, 10.f, "%.1f");
										sliderInt("max targets", &cfg::aimbot_projectile_max_sim_targets, 0, 10);
										endGroupBox();
									}
								}
								EndTable();
							}
						}
						if (sub == 3) {
							if (BeginTable("AimbotMeleeTable", 3)) {
								TableNextColumn();
								TableNextColumn();
								{
									if (groupBox("general")) {
										toggle("enabled", &cfg::aimbot_melee_active);
										selectSingle("aim type", &cfg::aimbot_melee_aim_method, { {"plain", 0}, {"smooth", 1}, {"silent", 2}, {"psilent", 3} });
										selectSingle("mode", &cfg::aimbot_melee_mode, { {"fov", 0}, {"distance", 1} });
										sliderFloat("fov", &cfg::aimbot_melee_fov, 1.f, 180.f);
										sliderFloat("smooth", &cfg::aimbot_melee_smooth, 1.f, 100.f);
										endGroupBox();
									}
								}
								EndTable();
							}
						}
					}
				}
			}
			ImGui::PopFont();
		}
		ImGui::End();
	}
}