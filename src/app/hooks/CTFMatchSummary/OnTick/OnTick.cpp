#include "../../../hooks.hpp"


MAKE_HOOK(
	CTFMatchSumary_OnTick,
	s::CTFMatchSumary_OnTick.get(),
	void,
	void* rcx)
{
	if (cfg::misc_remove_end_of_match_surveys)
	{
		const uintptr_t gc_client_system{ s::GTFGCClientSystem.call<uintptr_t>() };

		if (!gc_client_system) {
			return;
		}

		const int32_t offset{ *s::CMsgSurveyRequest_match_id_Offset.cast<int32_t*>() };

		uintptr_t* const msg_survey_request{ reinterpret_cast<uintptr_t*>(gc_client_system + offset) };

		if (msg_survey_request && *msg_survey_request & 2) {
			*msg_survey_request &= ~2;
		}
	}

	CALL_ORIGINAL(rcx);
}