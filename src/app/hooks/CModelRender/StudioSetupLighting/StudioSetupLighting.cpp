#include "../../../hooks.hpp"

MAKE_HOOK(
	CModelRender_StudioSetupLighting,
	s::CModelRender_StudioSetupLighting.get(),
	void,
	void* rcx, const DrawModelState_t& state, /*const*/ Vector& absEntCenter,
	LightCacheHandle_t* pLightcache, bool bVertexLit, bool bNeedsEnvCubemap, bool& bStaticLighting,
	DrawModelInfo_t& drawInfo, const ModelRenderInfo_t& pInfo, int drawFlags)
{
	if (cfg::static_player_model_lighting && pInfo.pRenderable)
	{
		if (IClientUnknown* const unknown{ pInfo.pRenderable->GetIClientUnknown() })
		{
			if (C_BaseEntity* const ent{ unknown->GetBaseEntity() })
			{
				if (C_BaseEntity* const owner{ ent->m_hOwnerEntity() })
				{
					if (owner->GetClassId() == class_ids::CTFPlayer) {
						absEntCenter = owner->GetRenderCenter();
					}
				}

				else
				{
					if (ent->GetClassId() == class_ids::CTFPlayer) {
						absEntCenter = ent->GetRenderCenter();
					}
				}
			}
		}
	}

	CALL_ORIGINAL(rcx, state, absEntCenter, pLightcache, bVertexLit, bNeedsEnvCubemap, bStaticLighting, drawInfo, pInfo, drawFlags);
}