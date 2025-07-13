#include "outlines.hpp"

#include "../cfg.hpp"
#include "../visual_utils/visual_utils.hpp"

#pragma warning (disable : 4244)

void Outlines::drawModel(IClientEntity *const ent, const bool model)
{
	if (!ent) {
		return;
	}

	m_drawing_models = true;

	tf_globals::block_mat_override = !model;

	if (ent->GetClassId() == class_ids::CTFPlayer)
	{
		if (C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() })
		{
			const float original_invis{ pl->m_flInvisibility() };

			if (original_invis > 0.99f)
			{
				pl->m_flInvisibility() = 0.0f;

				if (model) {
					i::render_view->SetBlend(0.0f);
				}
			}

			pl->DrawModel(model ? STUDIO_RENDER : (STUDIO_RENDER | STUDIO_NOSHADOWS));

			if (original_invis > 0.99f)
			{
				pl->m_flInvisibility() = original_invis;

				if (model) {
					i::render_view->SetBlend(1.0f);
				}
			}
		}
	}

	else {
		ent->DrawModel(model ? STUDIO_RENDER : (STUDIO_RENDER | STUDIO_NOSHADOWS));
	}

	if (model) {
		m_drawn_ents.insert(ent);
	}

	m_drawing_models = false;

	tf_globals::block_mat_override = false;
}

bool Outlines::onLoad()
{
	if (!i::engine->IsInGame() || !i::engine->IsConnected()) {
		return true;
	}

	if (!m_mat_glow_color) {
		m_mat_glow_color = i::mat_sys->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER);
	}

	if (!m_tex_rt_buff_0)
	{
		m_tex_rt_buff_0 = i::mat_sys->CreateNamedRenderTargetTextureEx
		(
			"tex_rt_buff_0",
			i::client->GetScreenWidth(),
			i::client->GetScreenHeight(),
			RT_SIZE_LITERAL,
			IMAGE_FORMAT_RGB888,
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
			CREATERENDERTARGETFLAGS_HDR
		);

		m_tex_rt_buff_0->IncrementReferenceCount();
	}

	if (!m_tex_rt_buff_1)
	{
		m_tex_rt_buff_1 = i::mat_sys->CreateNamedRenderTargetTextureEx
		(
			"tex_rt_buff_1",
			i::client->GetScreenWidth(),
			i::client->GetScreenHeight(),
			RT_SIZE_LITERAL,
			IMAGE_FORMAT_RGB888,
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
			CREATERENDERTARGETFLAGS_HDR
		);

		m_tex_rt_buff_1->IncrementReferenceCount();
	}

	if (!m_mat_halo_add_to_screen)
	{
		KeyValues *const kv{ new KeyValues("UnlitGeneric") };
		{
			kv->SetString("$basetexture", "tex_rt_buff_0");
			kv->SetString("$additive", "1");
		}
		
		m_mat_halo_add_to_screen = i::mat_sys->CreateMaterial("mat_halo_add_to_screen", kv);
	}

	if (!m_mat_halo_add_to_screen_fat)
	{
		// from hammer++
		KeyValues *const kv{ new KeyValues("screenspace_general") };
		{
			kv->SetString("$PIXSHADER", "haloaddoutline_ps20");
			kv->SetString("$ALPHA_BLEND_COLOR_OVERLAY", "1");
			kv->SetString("$basetexture", "tex_rt_buff_0");
			kv->SetString("$ignorez", "1");
			kv->SetString("$linearread_basetexture", "1");
			kv->SetString("$linearwrite", "1");
			kv->SetString("$texture1", "dev/glow_red");
			kv->SetString("$texture2", "dev/glow_white");
			kv->SetString("$texture3", "dev/glow_blue");
			kv->SetString("$additive", "1");
		}

		m_mat_halo_add_to_screen_fat = i::mat_sys->CreateMaterial("mat_halo_add_to_screen_fat", kv);
	}

	if (!m_mat_blur_x)
	{
		KeyValues *const kv{ new KeyValues("BlurFilterX") }; {
			kv->SetString("$basetexture", "tex_rt_buff_0");
		}
		
		m_mat_blur_x = i::mat_sys->CreateMaterial("mat_blur_x", kv);
	}

	if (!m_mat_blur_y)
	{
		KeyValues *const kv{ new KeyValues("BlurFilterY") }; {
			kv->SetString("$basetexture", "tex_rt_buff_1");
		}
		
		m_mat_blur_y = i::mat_sys->CreateMaterial("mat_blur_y", kv);
		m_bloom_amount = m_mat_blur_y->FindVar("$bloomamount", nullptr);
	}

	m_loaded = true;

	return true;
}

bool Outlines::onUnload()
{
	if (m_mat_halo_add_to_screen) {
		m_mat_halo_add_to_screen->DecrementReferenceCount();
		m_mat_halo_add_to_screen->DeleteIfUnreferenced();
		m_mat_halo_add_to_screen = nullptr;
	}

	if (m_mat_halo_add_to_screen_fat) {
		m_mat_halo_add_to_screen_fat->DecrementReferenceCount();
		m_mat_halo_add_to_screen_fat->DeleteIfUnreferenced();
		m_mat_halo_add_to_screen_fat = nullptr;
	}

	if (m_tex_rt_buff_0) {
		m_tex_rt_buff_0->DecrementReferenceCount();
		m_tex_rt_buff_0->DeleteIfUnreferenced();
		m_tex_rt_buff_0 = nullptr;
	}

	if (m_tex_rt_buff_1) {
		m_tex_rt_buff_1->DecrementReferenceCount();
		m_tex_rt_buff_1->DeleteIfUnreferenced();
		m_tex_rt_buff_1 = nullptr;
	}

	if (m_mat_blur_x) {
		m_mat_blur_x->DecrementReferenceCount();
		m_mat_blur_x->DeleteIfUnreferenced();
		m_mat_blur_x = nullptr;
	}

	if (m_mat_blur_y) {
		m_mat_blur_y->DecrementReferenceCount();
		m_mat_blur_y->DeleteIfUnreferenced();
		m_mat_blur_y = nullptr;
	}

	m_bloom_amount = nullptr;
	m_loaded = false;

	return true;
}

bool Outlines::onLevelInit()
{
	return onLoad();
}

bool Outlines::onLevelShutdown()
{
	return onUnload();
}

bool Outlines::onScreenSizeChange()
{
	if (!m_loaded) {
		return true;
	}

	if (onUnload()) {
		return onLoad();
	}

	return false;
}

void Outlines::run()
{
	m_drawn_ents.clear();
	m_outline_ents.clear();

	if (!cfg::outlines_active || !vis_utils->shouldRunVisuals() || !m_loaded) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	IMatRenderContext *const rc{ i::mat_sys->GetRenderContext() };

	if (!rc) {
		return;
	}

	const float original_blend{ i::render_view->GetBlend() };
	const Color original_clr{ i::render_view->GetColorModulation() };

	i::render_view->SetBlend(1.0f);
	i::render_view->SetColorModulation({ 255, 255, 255, 255 });

	ShaderStencilState_t sss_models{};

	sss_models.m_bEnable = true;
	sss_models.m_nReferenceValue = 1;
	sss_models.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
	sss_models.m_PassOp = STENCILOPERATION_REPLACE;
	sss_models.m_FailOp = STENCILOPERATION_KEEP;
	sss_models.m_ZFailOp = cfg::outlines_fill_occluded ? STENCILOPERATION_KEEP : STENCILOPERATION_REPLACE;

	sss_models.SetStencilState(rc);

	if (cfg::outlines_players_active)
	{
		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PLAYERS_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent || !ent->ShouldDraw() || !vis_utils->isEntOnScreen(local, ent)) {
				continue;
			}

			C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

			if (!pl || pl->deadflag()) {
				continue;
			}

			if (vis_utils->shouldSkipPlayer(
				local,
				pl,
				cfg::outlines_players_ignore_local,
				cfg::outlines_players_ignore_friends,
				cfg::outlines_players_ignore_enemies,
				cfg::outlines_players_ignore_teammates))
			{
				continue;
			}

			const Color clr{ vis_utils->getEntColor(pl) };

			m_outline_ents.emplace_back(OutlineEnt{ pl, clr, cfg::outlines_players_alpha });

			drawModel(pl, true);

			for (C_BaseEntity *attach{ pl->FirstMoveChild() }; attach; attach = attach->NextMovePeer())
			{
				if (!attach || !attach->ShouldDraw()) {
					continue;
				}

				m_outline_ents.emplace_back(OutlineEnt{ attach, clr, cfg::outlines_players_alpha });

				drawModel(attach, true);
			}
		}
	}

	if (cfg::outlines_buildings_active)
	{
		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::BUILDINGS_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent || !ent->ShouldDraw() || !vis_utils->isEntOnScreen(local, ent)) {
				continue;
			}

			C_BaseObject *const obj{ ent->cast<C_BaseObject>() };

			if (!obj) {
				continue;
			}

			if (vis_utils->shouldSkipBuilding(
				local,
				obj,
				cfg::outlines_buildings_ignore_local,
				cfg::outlines_buildings_ignore_enemies,
				cfg::outlines_buildings_ignore_teammates))
			{
				continue;
			}

			const Color clr{ vis_utils->getEntColor(obj) };

			m_outline_ents.emplace_back(OutlineEnt{ obj, clr, cfg::outlines_buildings_alpha });

			drawModel(obj, true);
		}
	}

	if (cfg::outlines_projectiles_active)
	{
		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PROJECTILES_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent || !ent->ShouldDraw() || !vis_utils->isEntOnScreen(local, ent)) {
				continue;
			}

			C_BaseEntity *const proj{ ent->cast<C_BaseEntity>() };

			if (!proj) {
				continue;
			}

			if (vis_utils->shouldSkipProjectile(
				local,
				proj,
				cfg::outlines_projectiles_ignore_local,
				cfg::outlines_projectiles_ignore_enemies,
				cfg::outlines_projectiles_ignore_teammates))
			{
				continue;
			}

			const Color clr{ vis_utils->getEntColor(proj) };

			m_outline_ents.emplace_back(OutlineEnt{ proj, clr, cfg::outlines_projectiles_alpha });

			drawModel(proj, true);
		}
	}

	if (cfg::outlines_world_active)
	{
		auto drawGroup = [&](const ECGroup group, const Color clr)
		{
			for (const EntityHandle_t ehandle : ec->getGroup(group))
			{
				IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

				if (!ent || !ent->ShouldDraw() || !vis_utils->isEntOnScreen(local, ent)) {
					continue;
				}

				m_outline_ents.emplace_back(OutlineEnt{ ent, clr, cfg::outlines_world_alpha });

				drawModel(ent, true);
			}
		};

		if (!cfg::outlines_world_ignore_health_packs) {
			drawGroup(ECGroup::HEALTH_PACKS, cfg::color_health_pack);
		}

		if (!cfg::outlines_world_ignore_ammo_packs) {
			drawGroup(ECGroup::AMMO_PACKS, cfg::color_ammo_pack);
		}

		if (!cfg::outlines_world_ignore_halloween_gifts) {
			drawGroup(ECGroup::HALLOWEEN_GIFTS, cfg::color_halloween_gift);
		}

		if (!cfg::outlines_world_ignore_mvm_money) {
			drawGroup(ECGroup::MVM_MONEY, cfg::color_mvm_money);
		}
	}

	ShaderStencilState_t{}.SetStencilState(rc);

	i::render_view->SetBlend(original_blend);
	i::render_view->SetColorModulation(original_clr);
}

void Outlines::drawEffect()
{
	if (m_outline_ents.empty() || !m_loaded) {
		return;
	}

	IMatRenderContext *const rc{ i::mat_sys->GetRenderContext() };

	if (!rc) {
		return;
	}

	if (cfg::outlines_style == 0) {
		m_bloom_amount->SetIntValue(cfg::outlines_bloom_amount);
	}

	const float original_blend{ i::render_view->GetBlend() };
	const Color original_clr{ i::render_view->GetColorModulation() };

	IMaterial *original_mat{};
	OverrideType_t original_mat_override{};

	i::model_render->GetMaterialOverride(&original_mat, &original_mat_override);
	i::model_render->ForcedMaterialOverride(m_mat_glow_color);

	const int w{ i::client->GetScreenWidth() };
	const int h{ i::client->GetScreenHeight() };

	rc->PushRenderTargetAndViewport();
	{
		rc->SetRenderTarget(m_tex_rt_buff_0);
		rc->Viewport(0, 0, w, h);
		rc->ClearColor4ub(0, 0, 0, 0);
		rc->ClearBuffers(true, false, false);

		for (const OutlineEnt &ent : m_outline_ents) {
			i::render_view->SetBlend(ent.alpha);
			i::render_view->SetColorModulation(ent.clr);
			drawModel(ent.ent, false);
		}
	}
	rc->PopRenderTargetAndViewport();

	if (cfg::outlines_style == 0)
	{
		rc->PushRenderTargetAndViewport();
		{
			rc->Viewport(0, 0, w, h);

			rc->SetRenderTarget(m_tex_rt_buff_1);
			rc->DrawScreenSpaceRectangle(m_mat_blur_x, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);

			rc->SetRenderTarget(m_tex_rt_buff_0);
			rc->DrawScreenSpaceRectangle(m_mat_blur_y, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		}
		rc->PopRenderTargetAndViewport();
	}

	ShaderStencilState_t sss_outlines{};

	sss_outlines.m_bEnable = true;
	sss_outlines.m_nWriteMask = 0x0;
	sss_outlines.m_nTestMask = 0xFF;
	sss_outlines.m_nReferenceValue = 0;
	sss_outlines.m_CompareFunc = STENCILCOMPARISONFUNCTION_EQUAL;
	sss_outlines.m_PassOp = STENCILOPERATION_KEEP;
	sss_outlines.m_FailOp = STENCILOPERATION_KEEP;
	sss_outlines.m_ZFailOp = STENCILOPERATION_KEEP;

	sss_outlines.SetStencilState(rc);

	if (cfg::outlines_style == 0) {
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
	}

	else if (cfg::outlines_style == 1) {
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen, -1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen, -1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen, 1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen, 1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
	}

	else if (cfg::outlines_style == 2) {
		rc->DrawScreenSpaceRectangle(m_mat_halo_add_to_screen_fat, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
	}

	ShaderStencilState_t{}.SetStencilState(rc);

	i::render_view->SetBlend(original_blend);
	i::render_view->SetColorModulation(original_clr);

	i::model_render->ForcedMaterialOverride(original_mat, original_mat_override);
}

bool Outlines::isDrawingModels() const
{
	return m_drawing_models;
}

bool Outlines::hasDrawn(IClientEntity *const ent) const
{
	return m_drawn_ents.contains(ent);
}