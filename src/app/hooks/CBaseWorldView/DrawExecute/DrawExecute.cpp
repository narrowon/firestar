#include "../../../hooks.hpp"

MAKE_HOOK(
	CBaseWorldView_DrawExecute,
	s::CBaseWorldView_DrawExecute.get(),
	void,
	void* rcx, float waterHeight, view_id_t viewID, float waterZAdjust)
{
	tf_globals::in_CBaseWorldView_DrawExecute = true;
	CALL_ORIGINAL(rcx, waterHeight, viewID, waterZAdjust);
	tf_globals::in_CBaseWorldView_DrawExecute = false;
}