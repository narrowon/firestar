#include "../../../hooks.hpp"

MAKE_HOOK(
	CStudioRender_DrawModelStaticProp,
	s::CStudioRender_DrawModelStaticProp.get(),
	void,
	void* rcx, const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld, int flags)
{
	if (cfg::mat_mod_active)
	{
		const float night_scale{ math::remap(cfg::mat_mod_night_scale, 0.0f, 1.0f, 1.0f, 0.1f) };

		i::studio_render->SetColorModulation(
			cfg::mat_mod_mode == 0 ? Color{ 255, 255, 255, 255 }.scaled(night_scale) : cfg::mat_mod_color_props
		);
	}

	CALL_ORIGINAL(rcx, drawInfo, modelToWorld, flags);
}