#include "../../../hooks.hpp"


MAKE_HOOK(
	C_OP_RenderSprites_RenderSpriteCard,
	s::C_OP_RenderSprites_RenderSpriteCard.get(),
	void,
	void* rcx, void* meshBuilder, void* pCtx, SpriteRenderInfo_t& info, int hParticle, void* pSortList, void* pCamera)
{
	if (cfg::particle_mod_active)
	{
		Color color{ cfg::particle_mod_color };

		if (cfg::particle_mod_mode == 1) {
			color = utils::rainbow(i::global_vars->curtime * cfg::particle_mod_rainbow_speed, 255);
		}

		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 0].m128_f32[hParticle & 0x3] = color.r / 255.0f;
		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 1].m128_f32[hParticle & 0x3] = color.g / 255.0f;
		info.m_pRGB[((hParticle / 4) * info.m_nRGBStride) + 2].m128_f32[hParticle & 0x3] = color.b / 255.0f;
	}

	CALL_ORIGINAL(rcx, meshBuilder, pCtx, info, hParticle, pSortList, pCamera);
}
