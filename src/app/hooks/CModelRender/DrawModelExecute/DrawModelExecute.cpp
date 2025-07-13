#include "../../../hooks.hpp"

MAKE_HOOK(
	CModelRender_DrawModelExecute,
	s::CModelRender_DrawModelExecute.get(),
	void,
	void* rcx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (IClientEntity* const client_ent{ i::ent_list->GetClientEntity(pInfo.entity_index) })
	{
		if (C_BaseEntity* const base_ent{ client_ent->GetBaseEntity() })
		{
			if (!outlines->isDrawingModels() && !lag_comp->isDrawingModels() && outlines->hasDrawn(base_ent)) {
				return;
			}
		}

		if (cfg::mat_mod_active && client_ent->GetClassId() == class_ids::CDynamicProp)
		{
			const float night_scale{ math::remap(cfg::mat_mod_night_scale, 0.0f, 1.0f, 1.0f, 0.1f) };

			i::render_view->SetColorModulation
			(
				cfg::mat_mod_mode == 0 ? Color{ 255, 255, 255, 255 }.scaled(night_scale) : cfg::mat_mod_color_props
			);

			CALL_ORIGINAL(rcx, state, pInfo, pCustomBoneToWorld);

			i::render_view->SetColorModulation({ 255, 255, 255, 255 });

			return;
		}

		if (cfg::remove_hands && client_ent->GetClassId() == class_ids::CTFViewModel) {
			return;
		}
	}

	CALL_ORIGINAL(rcx, state, pInfo, pCustomBoneToWorld);
}
