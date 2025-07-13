#include "../../../hooks.hpp"

MAKE_HOOK(
	CTFPartyClient_GetPartyMemberColor,
	s::CTFPartyClient_GetPartyMemberColor.get(),
	Color*,
	void* rcx, int index)
{
	Color* const result{ CALL_ORIGINAL(rcx, index) };

	if (cfg::misc_rainbow_party && result) {
		*result = utils::rainbow((i::global_vars->curtime * 2.0f) + index * 0.5f, 255);
	}

	return result;
}