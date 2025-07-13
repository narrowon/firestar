#pragma once

#include "../tf/interfaces.hpp"
#include <variant>
#include <mutex>

constexpr int POS_DEFAULT{ 0 };
constexpr int POS_LEFT{ 1 << 1 };
constexpr int POS_TOP{ 1 << 2 };
constexpr int POS_CENTERX{ 1 << 3 };
constexpr int POS_CENTERY{ 1 << 4 };
constexpr int POS_CENTERXY{ POS_CENTERX | POS_CENTERY };

namespace fonts {
	inline ImFont* font_esp = nullptr;
	inline ImFont* font_indicators = nullptr;
	inline ImFont* font_esp_small = nullptr;
	inline ImFont* font_menu_tabs = nullptr;
	inline ImFont* font_menu = nullptr;
	inline ImFont* font_icomoon = nullptr;
	inline ImFont* font_icomoon_widget = nullptr;
	inline ImFont* font_icomoon_widget2 = nullptr;
}

class Draw final
{
	IDirect3DStateBlock9* pixel_state = nullptr;
private:
	D3DMATRIX m_w2s_mat{};
	bool initialized;
public:
	void start();
	void initialize(IDirect3DDevice9* device);
	void render(ImDrawList* list);
	void swap_commands();
	void setup_states(IDirect3DDevice9* device);
	void backup_states(IDirect3DDevice9* device);
	void end();
	void updateW2S();
	vec2 getScreenSize();
	bool worldToScreen(const vec3 &world, vec2 &screen);
	//HFont getDefaultFont();

public:
	struct string_t {
		ImFont* font{};
		const vec2& pos{};
		std::string_view input{};
		Color      col{};
		const int	align{};
	};
	struct rect_t {
		const vec2& pos{};
		const vec2& size{};
		Color     col{};
	};
	struct filledrect_t {
		const vec2& pos{};
		const vec2& size{};
		Color     col{};
	};
	struct line_t {
		const vec2& from{};
		const vec2& to{};
		Color      col{};
	};
	struct circle_t {
		const vec2 pos{};
		const int radius{};
		Color col{};
	};
	struct circle_filled_t {
		const vec2 pos{};
		const int radius{};
		Color col{};
	};

	std::mutex m_mutex{};
	std::vector<std::variant<string_t, rect_t, filledrect_t, line_t, circle_t, circle_filled_t>> m_cmds{}, m_safe_cmds{};
public:
	void call_line(ImDrawList* list, const vec2 &from, const vec2 &to, const Color clr);
	void  call_rect(ImDrawList* list, const vec2 &pos, const vec2 &size, const Color clr);
	void  call_rectFilled(ImDrawList* list, const vec2 &pos, const vec2 &size, const Color clr);
	void  call_rectHSV(ImDrawList* list, const vec2 &pos, const vec2 &size, const Color clr);
	void  call_circle(ImDrawList* list, const vec2 pos, const int radius, const Color clr);
	void  call_circleFilled(ImDrawList* list, const vec2 pos, const int radius, const Color clr);
	void  call_string(ImDrawList* list, ImFont* font,const vec2 &pos, std::string_view str, const Color clr, const int align = 0);
	void line(const vec2& from, const vec2& to, const Color clr);
	void  rect(const vec2& pos, const vec2& size, const Color clr);
	void  rectFilled(const vec2& pos, const vec2& size, const Color clr);
	void  circle(const vec2 pos, const int radius, const Color clr);
	void  circleFilled(const vec2 pos, const int radius, const Color clr);
	void  string(ImFont* font, const vec2& pos, std::string_view str, const Color clr, const int align = 0);
};

MAKE_UNIQUE(Draw, draw);