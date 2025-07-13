#include "widgets.hpp"
#include "../../../shared/libs/imgui/imgui_internal.hpp"
#include "ui.hpp"
#include "../cfg.hpp"

#undef min
#undef max
using namespace ImGui;

struct Row_t
{
	ImVec2 m_vPos;
	ImVec2 m_vSize;
};
static ImVec2 GetDrawPos()
{
	return GetWindowPos() - GetCurrentWindow()->Scroll;
}
static float GetDrawPosX()
{
	return GetWindowPos().x - GetCurrentWindow()->Scroll.x;
}
static float GetDrawPosY()
{
	return GetWindowPos().y - GetCurrentWindow()->Scroll.y;
}

static std::vector<Row_t> vRowSizes;
static void AddRowSize(ImVec2 vPos, ImVec2 vSize)
{
	if (GetCurrentWindow()->Flags & ImGuiWindowFlags_Popup)
		return;

	vPos += GetDrawPos();
	if (!vRowSizes.empty() && vRowSizes.back().m_vPos.y != vPos.y)
		vRowSizes.clear();
	vRowSizes.emplace_back(vPos, vSize);
}
static float GetRowPos(bool bDrawPos = false)
{
	if (!vRowSizes.empty())
		return vRowSizes.front().m_vPos.y - (!bDrawPos ? GetDrawPos().y : 0.f);
	else
		return GetCursorPosY() + (bDrawPos ? GetDrawPos().y : 0.f);
}
static float GetRowSize(float flDefault = 0.f)
{
	if (GetCurrentWindow()->Flags & ImGuiWindowFlags_Popup)
		return flDefault;

	float flMax = 0.f;
	for (auto& [_, vSize] : vRowSizes)
		flMax = std::max(flMax, vSize.y);
	return flMax;
}

static std::unordered_map<uint32_t, float> mLastHeights;
static std::vector<uint32_t> vStoredLabels;

bool toggle(const char* const label, bool* const var)
{
	const bool callback{ ImGui::Checkbox(label, var) };

	//bindWidget(var, std::make_shared<WidgetCtx>(std::make_any<bool>(false), std::make_any<bool>(true)).get());

	return callback;
}

bool menuTab(const char* label, bool active)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	const ImVec2 size(95, 23);
	float total_width = ImGui::GetWindowContentRegionMin().x;
	float spacing = style.ItemInnerSpacing.x;
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 pos = ImVec2(cursor_pos.x - total_width, cursor_pos.y + style.FramePadding.y);


	const ImRect bb(pos, pos + size);
	const ImGuiID id = window->GetID(label);
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Button behavior
	bool hovered, held;
	bool bActive = false;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// Draw background and border
	ImColor first = menu->menu_accent;
	ImColor second = menu->menu_subaccent;
	
	ImU32 border_color = second;
	ImU32 accent_color = first;
	ImU32 text_color = active ? accent_color : ImGui::GetColorU32(ImVec4(0.71f, 0.71f, 0.71f, 1.0f)); // ~180,180,180
	ImU32 bg_color = ImGui::GetColorU32(ImVec4(28 / 255.f, 28 / 255.f, 28 / 255.f, 255 / 255.f));
	//window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_color);

	// Draw label
	ImGui::PushFont(fonts::font_menu_tabs);
	ImVec2 text_size = ImGui::CalcTextSize(label);
	ImVec2 text_pos = bb.Min + (size - text_size) * 0.5f;
	window->DrawList->AddText(text_pos, text_color, label);
	ImGui::PopFont();

	// Handle click sound
	static float time_pressed = 0.0f;
	if (pressed)
	{
		time_pressed = ImGui::GetTime();
		bActive = true;
	}

	return bActive;
}

bool menuSubTab(const char* label, bool active)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	const ImVec2 size(80, 23);
	ImVec2 pos = window->DC.CursorPos;

	// Calculate rectangle bounds
	const ImRect bb(pos, pos + size);
	const ImGuiID id = window->GetID(label);
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Button behavior
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	bool bActive = false;

	ImColor first = menu->menu_accent;
	ImColor second = menu->menu_subaccent;
	// Draw background and border
	ImU32 bg_color = ImGui::GetColorU32(ImVec4(28 / 255.f, 28 / 255.f, 28 / 255.f, 255 / 255.f));
	ImU32 border_color = second;
	ImU32 accent_color = first;
	
	ImU32 text_color = active ? accent_color : ImGui::GetColorU32(ImVec4(0.71f, 0.71f, 0.71f, 1.0f)); // ~180,180,180

	//window->DrawList->AddRectFilled(bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), bg_color);

	//window->DrawList->AddRect(bb.Min, bb.Max, accent_color);

// Draw label
	ImGui::PushFont(fonts::font_menu_tabs);
	ImVec2 text_size = ImGui::CalcTextSize(label);
	ImVec2 text_pos = bb.Min + (size - text_size) * 0.5f;
	window->DrawList->AddText(text_pos, text_color, label);
	ImGui::PopFont();
	static float time_pressed = 0.0f;
	if (pressed)
	{
		time_pressed = ImGui::GetTime();
		bActive = true;
	}

	return bActive;
}

bool groupBox(const char* sLabel, float flPaddingMod, float flMinHeight, bool bForceHeight, uint32_t uHash)
{
	ImColor first = menu->menu_accent;
	ImColor second = menu->menu_subaccent;

	if (!uHash)
		uHash = fnv1a::hash32(sLabel);
	vStoredLabels.push_back(uHash);

	if (!bForceHeight && mLastHeights.contains(uHash) && mLastHeights[uHash] > flMinHeight)
		flMinHeight = mLastHeights[uHash];
	PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });

	bool bReturn = BeginChild(sLabel, { GetColumnWidth(), 0 }, false, ImGuiWindowFlags_AlwaysUseWindowPadding /*| ImGuiWindowFlags_AlwaysVerticalScrollbar*/);
	if (bReturn)
	{
		ImGui::PushFont(fonts::font_menu);
		auto vDrawPos = GetDrawPos();

		GetCurrentWindow()->DrawList->AddRectFilledMultiColor(
			{ vDrawPos.x + 1.0f, vDrawPos.y },
			{ vDrawPos.x + GetWindowSize().x - 1.0f, vDrawPos.y + GetWindowSize().y },
			first, second,
			second, second
		);

		GetCurrentWindow()->DrawList->AddRectFilledMultiColor(
			{ vDrawPos.x, vDrawPos.y + 1.0f },
			{ vDrawPos.x + GetWindowSize().x , vDrawPos.y + GetWindowSize().y - 1.0f },
			first,first,
			second, second
		);

		GetCurrentWindow()->DrawList->AddRectFilled(
			{ vDrawPos.x + 2.5f, vDrawPos.y + 2.5f },
			{ vDrawPos.x + GetWindowSize().x - 2.5f, vDrawPos.y + GetWindowSize().y - 2.5f },
			ImColor(20, 20, 20)
		);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 0 });

		ImVec2 vOriginalPos = GetCursorPos();

		SetCursorPosX(vOriginalPos.x);

		ImGui::TextColored(first, sLabel);
		ImGui::PopFont();
		SetCursorPos(vOriginalPos); ImGui::Dummy({ 0, 21 + flPaddingMod });
	}

	if (flPaddingMod)
		SetCursorPosY(GetCursorPosY() + flPaddingMod);

	return bReturn;
}

bool endGroupBox()
{
	uint32_t uHash = vStoredLabels.back();
	vStoredLabels.pop_back();
	float flHeight = GetItemRectMax().y - GetWindowPos().y;
	if (flHeight > 0.f)
		mLastHeights[uHash] = flHeight + GetStyle().WindowPadding.y;

	PopStyleVar();
	EndChild();
	PopStyleVar();
	return true;
}

bool selectSingle(const char* const label, int* const var, std::vector<std::pair<std::string, int>> vars)
{
	bool callback{};

	if (ImGui::BeginCombo(label, vars[*var].first.c_str()))
	{
		for (int i{}; i < static_cast<int>(vars.size()); i++)
		{
			const bool selected{ i == *var };

			if (ImGui::Selectable(vars[i].first.c_str(), selected)) {
				*var = i;
				callback = true;
			}

			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

//	bindWidget(var, std::make_shared<WidgetCtxSelectSingle>(vars).get());

	return callback;
}

bool selectMulti(const char* const label, const std::vector<std::pair<std::string, bool*>> vars)
{
	bool callback{};

	std::string preview{};

	for (const std::pair<std::string, bool*>& var : vars)
	{
		if (*var.second)
		{
			if (!preview.empty()) {
				preview += ", ";
			}

			preview += var.first;
		}
	}

	if (ImGui::BeginCombo(label, preview.c_str()))
	{
		for (const std::pair<std::string, bool*>& var : vars)
		{
			if (ImGui::Selectable(var.first.c_str(), false, ImGuiSelectableFlags_NoAutoClosePopups)) {
				*var.second = !*var.second;
			}

			/*if (ImGui::SelectableWithValueOnRight(var.first.c_str(), false, std::format("{}", *var.second).c_str(), ImGuiSelectableFlags_NoAutoClosePopups)) {
				*var.second = !*var.second;
			}*/

			//bindWidget(var.second, std::make_shared<WidgetCtx>(std::make_any<bool>(false), std::make_any<bool>(true)).get());
		}

		ImGui::EndCombo();
	}

	return callback;
}

//menu->menu_accent.Value.x, menu->menu_accent.Value.y, menu->menu_accent.Value.z

const char* keys[] = {
	"[-]",
	"[M1]",
	"[M2]",
	"[CN]",
	"[M3]",
	"[M4]",
	"[M5]",
	"[-]",
	"[BAC]",
	"[TAB]",
	"[-]",
	"[-]",
	"[CLR]",
	"[RET]",
	"[-]",
	"[-]",
	"[SHI]",
	"[CTL]",
	"[ALT]",
	"[PAU]",
	"[CAP]",
	"[KAN]",
	"[-]",
	"[JUN]",
	"[FIN]",
	"[KAN]",
	"[-]",
	"[ESC]",
	"[CON]",
	"[NCO]",
	"[ACC]",
	"[MAD]",
	"[SPA]",
	"[PGU]",
	"[PGD]",
	"[END]",
	"[HOM]",
	"[LEF]",
	"[UP]",
	"[RIG]",
	"[DOW]",
	"[SEL]",
	"[PRI]",
	"[EXE]",
	"[PRI]",
	"[INS]",
	"[DEL]",
	"[HEL]",
	"[0]",
	"[1]",
	"[2]",
	"[3]",
	"[4]",
	"[5]",
	"[6]",
	"[7]",
	"[8]",
	"[9]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[A]",
	"[B]",
	"[C]",
	"[D]",
	"[E]",
	"[F]",
	"[G]",
	"[H]",
	"[I]",
	"[J]",
	"[K]",
	"[L]",
	"[M]",
	"[N]",
	"[O]",
	"[P]",
	"[Q]",
	"[R]",
	"[S]",
	"[T]",
	"[U]",
	"[V]",
	"[W]",
	"[X]",
	"[Y]",
	"[Z]",
	"[WIN]",
	"[WIN]",
	"[APP]",
	"[-]",
	"[SLE]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[MUL]",
	"[ADD]",
	"[SEP]",
	"[MIN]",
	"[DEC]",
	"[DIV]",
	"[F1]",
	"[F2]",
	"[F3]",
	"[F4]",
	"[F5]",
	"[F6]",
	"[F7]",
	"[F8]",
	"[F9]",
	"[F10]",
	"[F11]",
	"[F12]",
	"[F13]",
	"[F14]",
	"[F15]",
	"[F16]",
	"[F17]",
	"[F18]",
	"[F19]",
	"[F20]",
	"[F21]",
	"[F22]",
	"[F23]",
	"[F24]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[NUM]",
	"[SCR]",
	"[EQU]",
	"[MAS]",
	"[TOY]",
	"[OYA]",
	"[OYA]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[SHI]",
	"[SHI]",
	"[CTR]",
	"[CTR]",
	"[ALT]",
	"[ALT]"
};

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_MBUTTON        0x04
#define VK_XBUTTON1       0x05
#define VK_XBUTTON2       0x06
#define VK_BACK           0x08
#define VK_RMENU          0xA5

bool hotkey(const char* label, int* current_key, int* keystyle)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	SameLine(window->Size.x - 28 - 5 + 1);

	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	ImGuiIO* io = &GetIO();

	const ImVec2 label_size = CalcTextSize(keys[*current_key]);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + label_size);
	const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(window->Pos.x + window->Size.x - window->DC.CursorPos.x, label_size.y));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	const bool hovered = IsItemHovered();
	const bool edit_requested = hovered && io->MouseClicked[0];
	const bool style_requested = hovered && io->MouseClicked[1];

	if (edit_requested) {
		if (g.ActiveId != id) {
			memset(io->MouseDown, 0, sizeof(io->MouseDown));
			memset(io->KeysDown, 0, sizeof(io->KeysDown));
			*current_key = 0;
		}

		SetActiveID(id, window);
		FocusWindow(window);
	}
	else if (!hovered && io->MouseClicked[0] && g.ActiveId == id)
		ClearActiveID();

	bool value_changed = false;
	int key = *current_key;

	if (g.ActiveId == id) {
		for (auto i = 0; i < 5; i++) {
			if (io->MouseDown[i]) {
				switch (i) {
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
				}
				value_changed = true;
				ClearActiveID();
			}
		}

		if (!value_changed) {
			for (auto i = VK_BACK; i <= VK_RMENU; i++) {
				if (io->KeysDown[i]) {
					key = i;
					value_changed = true;
					ClearActiveID();
				}
			}
		}

		if (IsKeyPressed(ImGuiKey_Escape)) {
			*current_key = 0;
			ClearActiveID();
		}
		else
			*current_key = key;
	}
	else {
		if (keystyle) {
			bool popup_open = IsPopupOpen(id, 0);

			if (style_requested && !popup_open)
				OpenPopupEx(id);

			if (popup_open) {
				SetNextWindowSize(ImVec2(100, ImGui::GetTextLineHeightWithSpacing() * 4));

				char name[16];
				ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

				// Peak into expected window size so we can position it
				if (ImGuiWindow* popup_window = FindWindowByName(name))
					if (popup_window->WasActive)
					{
						// Generate unique popup ID
						ImGuiID popup_id = ImGui::GetID("##Combo");
						// Calculate the expected size of the popup
						ImVec2 size_expected = ImGui::GetWindowSize();

						// Get the reference position for the popup
						ImVec2 ref_pos = ImGui::GetCursorScreenPos();

						// Define the outer rectangle and the rectangle to avoid
						ImRect r_outer = ImGui::GetPopupAllowedExtentRect(ImGui::GetCurrentWindow());
						ImRect r_avoid = ImRect(ref_pos, ref_pos + size_expected);
						// Determine the best position for the popup
						ImGuiDir last_dir = ImGuiDir_None; // Initialize last direction
						ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(ref_pos, size_expected, &last_dir, r_outer, r_avoid, ImGuiPopupPositionPolicy_ComboBox);

						ImGui::SetNextWindowPos(pos);
					}

				// Horizontally align ourselves with the framed text
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
				PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
				PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
				PushStyleColor(ImGuiCol_Border, ImVec4(ImColor(51, 51, 51)));
				PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImColor(15, 15, 15)));
				bool ret = Begin(name, NULL, window_flags);
				PopStyleColor(2);
				PopStyleVar(2);

				if (Selectable("always on", *keystyle == 0))
					*keystyle = 0;

				if (Selectable("on hotkey", *keystyle == 1))
					*keystyle = 1;

				if (Selectable("toggle", *keystyle == 2))
					*keystyle = 2;

				if (Selectable("off hotkey", *keystyle == 3))
					*keystyle = 3;

				EndPopup();
			}
		}
	}

	char buf_display[64] = "[-]";

	if (*current_key != 0 && g.ActiveId != id)
		strcpy_s(buf_display, keys[*current_key]);
	else if (g.ActiveId == id)
		strcpy_s(buf_display, "[-]");

	PushFont(fonts::font_menu);
	window->DrawList->AddText(frame_bb.Min, g.ActiveId == id ? ImColor(255 / 255.f, 16 / 255.f, 16 / 255.f, g.Style.Alpha) : ImColor(90 / 255.f, 90 / 255.f, 90 / 255.f, g.Style.Alpha), buf_display);
	PopFont();

	return value_changed;
}

bool sliderInt(const char* const label, int* const var, int min, int max, const char* const fmt)
{
	const bool callback{ ImGui::SliderInt(label, var, min, max, fmt, ImGuiSliderFlags_AlwaysClamp) };

//	bindWidget(var, std::make_shared<WidgetCtxSliderInt>(min, max, fmt).get());

	return callback;
}

bool sliderFloat(const char* const label, float* const var, float min, float max, const char* const fmt)
{
	const bool callback{ ImGui::SliderFloat(label, var, min, max, fmt, ImGuiSliderFlags_AlwaysClamp) };

	//bindWidget(var, std::make_shared<WidgetCtxSliderFloat>(min, max, fmt).get());

	return callback;
}

bool colorPicker(const char* const label, Color* const var)
{
	ImVec4 clr{};

	clr.x = static_cast<float>(var->r) / 255.0f;
	clr.y = static_cast<float>(var->g) / 255.0f;
	clr.z = static_cast<float>(var->b) / 255.0f;
	clr.w = static_cast<float>(var->a) / 255.0f;

	const bool callback{ ImGui::ColorEdit4(label, &clr.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar) };

	var->r = static_cast<uint8_t>(clr.x * 255.0f);
	var->g = static_cast<uint8_t>(clr.y * 255.0f);
	var->b = static_cast<uint8_t>(clr.z * 255.0f);
	var->a = static_cast<uint8_t>(clr.w * 255.0f);

	return callback;
}

bool inputString(const char* const label, std::string* const var, const char* const fmt, int flags)
{
	return ImGui::InputText(label, var->data(), var->capacity() + 1, ImGuiInputTextFlags_CallbackResize | flags, 0, var);
}

bool inputStringWithHint(const char* const label, std::string* const var, const char* const hint, const char* const fmt, int flags)
{
	return ImGui::InputTextWithHint(label, hint, var->data(), var->capacity() + 1, ImGuiInputTextFlags_CallbackResize | flags, 0, var);
}