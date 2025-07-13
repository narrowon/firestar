#pragma once

#include "../../../game/game.hpp"

class Outlines final : public HasLoad, public HasUnload, public HasLevelInit, public HasLevelShutdown, public HasScreenSizeChange
{
private:
	struct OutlineEnt final
	{
		IClientEntity *ent{};
		Color clr{};
		float alpha{};
	};

private:
	bool m_loaded{};
	bool m_drawing_models{};

private:
	IMaterial *m_mat_glow_color{};
	IMaterial *m_mat_halo_add_to_screen{};
	IMaterial *m_mat_halo_add_to_screen_fat{};
	IMaterial *m_mat_blur_x{};
	IMaterial *m_mat_blur_y{};

private:
	IMaterialVar *m_bloom_amount{};

private:
	ITexture *m_tex_rt_buff_0{};
	ITexture *m_tex_rt_buff_1{};

private:
	std::unordered_set<IClientEntity *> m_drawn_ents{};
	std::vector<OutlineEnt> m_outline_ents{};

private:
	void drawModel(IClientEntity *const ent, const bool model);

public:
	bool onLoad() override;
	bool onUnload() override;
	bool onLevelInit() override;
	bool onLevelShutdown() override;
	bool onScreenSizeChange() override;

public:
	void run(); //draws models (stencils them) and adds outline entities
	void drawEffect(); //draws outline entities to the screen

public:
	bool isDrawingModels() const;
	bool hasDrawn(IClientEntity *const ent) const;
};

MAKE_UNIQUE(Outlines, outlines);