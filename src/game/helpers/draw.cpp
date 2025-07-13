#include "draw.hpp"
#include "verdana.h"
#include "tahoma_bold.h"
#include "font_icons.h"
#include "tahoma.h"

using namespace ImGui;

void Draw::start()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();
}

void Draw::initialize(IDirect3DDevice9* device)
{
	if (!initialized)
	{
		CreateContext();
		ImGui_ImplWin32_Init(FindWindowA("Valve001", nullptr));
		ImGui_ImplDX9_Init(device);
		//console.print("initialized imgui");

		StyleColorsDark();

		ImGuiIO& io = GetIO();

		ImFontConfig cfg = {};

		cfg.PixelSnapH = true;
		cfg.OversampleH = cfg.OversampleV = 1;
		cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
		cfg.FontDataOwnedByAtlas = false;

		/*console.print("setup fonts");*/
		fonts::font_esp = io.Fonts->AddFontFromMemoryTTF(verdana, sVerdana, 12.0f, &cfg, io.Fonts->GetGlyphRangesDefault());
		fonts::font_indicators = io.Fonts->AddFontFromMemoryTTF(verdana, sVerdana, 12.0f, &cfg, io.Fonts->GetGlyphRangesDefault());
		fonts::font_esp_small = io.Fonts->AddFontFromMemoryTTF(tahoma_bold, sizeof(tahoma_bold), 10.0f, &cfg, io.Fonts->GetGlyphRangesDefault());

		fonts::font_menu_tabs = io.Fonts->AddFontFromMemoryTTF(tahoma, sTahoma, 12.0f, &cfg, io.Fonts->GetGlyphRangesDefault());
		fonts::font_menu = io.Fonts->AddFontFromMemoryTTF(tahoma, sTahoma, 11.0f, &cfg, io.Fonts->GetGlyphRangesDefault());

		fonts::font_icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::font_icomoon_widget = io.Fonts->AddFontFromMemoryTTF(icomoon_widget, sizeof(icomoon_widget), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::font_icomoon_widget2 = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		initialized = true;
		//console.print("rendering is done");
	}
}

void Draw::render(ImDrawList* list)
{
	std::scoped_lock _{ m_mutex };

	for (auto&& cmds : m_safe_cmds) {
		if (auto* string = std::get_if<string_t>(&cmds)) {
			call_string(list, string->font, string->pos, string->input, string->col, string->align);
		}
		if (auto* rect = std::get_if<rect_t>(&cmds)) {
			call_rect(list, rect->pos, rect->size, rect->col);
		}
		if (auto* filledrect = std::get_if<filledrect_t>(&cmds)) {
			call_rectFilled(list, filledrect->pos, filledrect->size, filledrect->col);
		}
		if (auto* line = std::get_if<line_t>(&cmds)) {
			call_line(list, line->from, line->to, line->col);
		}
		if (auto* circle = std::get_if<circle_t>(&cmds)) {
			call_circle(list, circle->pos, circle->radius, circle->col);
		}
		if (auto* circle = std::get_if<circle_filled_t>(&cmds)) {
			call_circle(list, circle->pos, circle->radius, circle->col);
		}
	}
}

void Draw::swap_commands()
{
	{
		std::scoped_lock _{ m_mutex };
		m_cmds.swap(m_safe_cmds);
	}

	m_cmds.clear();
}

void Draw::setup_states(IDirect3DDevice9* device)
{
	device->CreateStateBlock(D3DSBT_ALL, &pixel_state);

	pixel_state->Capture();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

void Draw::backup_states(IDirect3DDevice9* device)
{
	if (!pixel_state)
		return;

	pixel_state->Apply();
	pixel_state->Release();
}

void Draw::end()
{
	ImGui::EndFrame();
	ImGui::Render();

	ImDrawData* draw_data = GetDrawData();

	ImGui_ImplDX9_RenderDrawData(draw_data);
}

void Draw::updateW2S()
{
	CViewSetup view{};

	if (!i::client->GetPlayerView(view)) {
		return;
	}

	D3DMATRIX world_to_view{};
	D3DMATRIX view_to_projection{};
	D3DMATRIX world_to_pixels{};

	i::render_view->GetMatricesForView(view, &world_to_view, &view_to_projection, &m_w2s_mat, &world_to_pixels);
}

vec2 Draw::getScreenSize()
{
	return vec2
	{
		static_cast<float>(i::client->GetScreenWidth()),
		static_cast<float>(i::client->GetScreenHeight())
	};
}

bool Draw::worldToScreen(const vec3 &world, vec2 &screen)
{
	const D3DMATRIX &w2s{ m_w2s_mat };

#pragma warning (push)
#pragma warning (disable : 6385)

	const float w{ w2s.m[3][0] * world[0] + w2s.m[3][1] * world[1] + w2s.m[3][2] * world[2] + w2s.m[3][3] };

#pragma warning (pop)

	if (w <= 0.001f) {
		return false;
	}

	const vec2 ss{ getScreenSize() };

	screen.x = (ss.x / 2.0f) + (0.5f * ((w2s.m[0][0] * world[0] + w2s.m[0][1] * world[1] + w2s.m[0][2] * world[2] + w2s.m[0][3]) * (1.0f / w)) * ss.x + 0.5f);
	screen.y = (ss.y / 2.0f) - (0.5f * ((w2s.m[1][0] * world[0] + w2s.m[1][1] * world[1] + w2s.m[1][2] * world[2] + w2s.m[1][3]) * (1.0f / w)) * ss.y + 0.5f);

	return true;
}



void Draw::line(const vec2 &from, const vec2 &to, const Color clr)
{
	m_cmds.emplace_back(line_t{ from,to, clr });
}

void Draw::rect(const vec2 &pos, const vec2 &size, const Color clr)
{
	m_cmds.emplace_back(rect_t{ pos,size, clr });
}

void Draw::rectFilled(const vec2 &pos, const vec2 &size, const Color clr)
{
	m_cmds.emplace_back(filledrect_t{pos, size, clr });
}

void Draw::circle(const vec2 pos, const int radius, const Color clr)
{
	m_cmds.emplace_back(circle_t{ pos, radius, clr });
}

void Draw::circleFilled(const vec2 pos, const int radius, const Color clr)
{
	m_cmds.emplace_back(circle_filled_t{ pos, radius, clr });
}

void Draw::string(ImFont* font, const vec2& pos, std::string_view str, const Color clr, const int align)
{
	m_cmds.emplace_back(string_t{font, pos, str, clr, align });
}

void Draw::call_line(ImDrawList* list, const vec2& from, const vec2& to, const Color clr)
{
	auto f_x1 = from.x;
	auto f_y1 = from.y;
	auto f_x2 = to.x;
	auto f_y2 = to.y;

	list->AddLine({ f_x1 + 1.f, f_y1 + 1.f }, { f_x1 + f_x2 - 1.f, f_y1 + f_y2 - 1.f },
		ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddLine({ f_x1 - 1.f, f_y1 - 1.f }, { f_x1 + f_x2 + 1.f, f_y1 + f_y2 + 1.f },
		ImColor(0.f, 0.f, 0.f, clr.a / 255.f));

	list->AddLine({ f_x1, f_y1 }, { f_x1 + f_x2, f_y1 + f_y2 }, ImColor(clr.r, clr.g, clr.b, clr.a));
}

void Draw::call_rect(ImDrawList* list, const vec2& pos, const vec2& size, const Color clr)
{
	auto f_x = static_cast<float>(pos.x);
	auto f_y = static_cast<float>(pos.y);
	auto f_w = static_cast<float>(size.x);
	auto f_h = static_cast<float>(size.y);

	list->AddRect({ f_x + 1.f, f_y + 1.f }, { f_x + f_w - 1.f, f_y + f_h - 1.f },
		ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddRect({ f_x - 1.f, f_y - 1.f }, { f_x + f_w + 1.f, f_y + f_h + 1.f },
		ImColor(0.f, 0.f, 0.f, clr.a / 255.f));

	list->AddRect({ f_x, f_y }, { f_x + f_w, f_y + f_h }, ImColor(clr.r, clr.g, clr.b, clr.a));
}

void Draw::call_rectFilled(ImDrawList* list, const vec2& pos, const vec2& size, const Color clr)
{
	auto f_x = static_cast<float>(pos.x);
	auto f_y = static_cast<float>(pos.y);
	auto f_w = static_cast<float>(size.x);
	auto f_h = static_cast<float>(size.y);

	list->AddRectFilled({ f_x + 1.f, f_y + 1.f }, { f_x + f_w + 1.f, f_y + f_h + 1.f }, ImColor(clr.r, clr.g, clr.b, clr.a));
}

void Draw::call_circle(ImDrawList* list, const vec2 pos, const int radius, const Color clr)
{
	auto f_x = static_cast<float>(pos.x);
	auto f_y = static_cast<float>(pos.y);

	list->AddCircle({ f_x, f_y }, radius - 1.f, ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddCircle({ f_x, f_y }, radius + 1.f, ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddCircle({ f_x, f_y }, radius, ImColor(clr.r, clr.g, clr.b, clr.a));
}

void Draw::call_circleFilled(ImDrawList* list, const vec2 pos, const int radius, const Color clr)
{
	auto f_x = static_cast<float>(pos.x);
	auto f_y = static_cast<float>(pos.y);

	list->AddCircleFilled({ f_x, f_y }, radius - 1.f, ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddCircleFilled({ f_x, f_y }, radius + 1.f, ImColor(0.f, 0.f, 0.f, clr.a / 255.f));
	list->AddCircleFilled({ f_x, f_y }, radius, ImColor(clr.r, clr.g, clr.b, clr.a));
}

void Draw::call_string(ImDrawList* list, ImFont* font, const vec2& pos, std::string_view str, const Color clr, const int align)
{
	if (!str.data())
		return;

	float  left, top;
	auto   f_x = static_cast<float>(pos.x);
	auto   f_y = static_cast<float>(pos.y);

	ImGui::PushFont(font);
	ImVec2 size = ImGui::CalcTextSize(str.data(), nullptr, false);

	//if (align == text_align::Center_X) {
	//	f_x -= size.x / 2;
	//}
	//if (align == text_align::Center_Y) {
	//	f_y -= size.y / 2;
	//}

	//if (align == text_align::Reverse_X) {
	//	f_x -= size.x;
	//}

	//if (align == text_align::Default) {
	//	//do nothing
	//}

	if (align & POS_LEFT) {
		f_x -= size.x;
	}

	if (align & POS_TOP) {
		f_y -= size.y;
	}

	if (align & POS_CENTERX) {
		f_x -= size.x / 2;
	}

	if (align & POS_CENTERY) {
		f_y -= size.y / 2;
	}

	left = f_x;
	top = f_y + 1.f;
	list->AddText({ left, top }, ImColor(0.f, 0.f, 0.f, clr.a / 255.f), str.data(), nullptr);
	left = f_x + 1.f;
	top = f_y;
	list->AddText( { left, top }, ImColor(0.f, 0.f, 0.f, clr.a / 255.f), str.data(), nullptr);
	left = f_x - 1.f;
	top = f_y;
	list->AddText( { left, top }, ImColor(0.f, 0.f, 0.f, clr.a / 255.f), str.data(), nullptr);
	left = f_x;
	top = f_y - 1.f;
	list->AddText( { left, top }, ImColor(0.f, 0.f, 0.f, clr.a / 255.f), str.data(), nullptr);
	left = f_x;
	top = f_y;
	list->AddText( { left, top }, ImColor(clr.r, clr.g, clr.b, clr.a), str.data(), nullptr);
	ImGui::PopFont();

}
