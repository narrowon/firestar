#include "../../../hooks.hpp"


MAKE_HOOK(
	C_TFPlayer_UpdateClientSideAnimation,
	s::C_TFPlayer_UpdateClientSideAnimation.get(),
	void,
	void* rcx)
{
	if (C_TFPlayer* const pl{ reinterpret_cast<C_TFPlayer*>(rcx) })
	{
		if (pl->entindex() != i::engine->GetLocalPlayer() && !pl->InCond(TF_COND_TAUNTING))
		{
			CStudioHdr* hdr = pl->GetModelPtr();
			if (!hdr || cfg::remove_interp)
				return;

			for (int iter = 0; iter < pl->m_CachedBoneData().Count(); iter++)
			{
				matrix3x4_t& bone = pl->m_CachedBoneData().Element(iter);
				vec3 origin_offset{ bone[0][0], bone[0][1], bone[0][2] };

				origin_offset -= pl->m_vecOrigin();
				origin_offset += pl->GetRenderOrigin();

				bone[0][0] = origin_offset.x;
				bone[0][1] = origin_offset.y;
				bone[0][2] = origin_offset.z;
			}

			pl->SetupBones_AttachmentHelper(hdr);

			return;
		}
	}

	CALL_ORIGINAL(rcx);
}