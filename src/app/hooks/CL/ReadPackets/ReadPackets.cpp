#include "../../../hooks.hpp"

MAKE_HOOK(
	CL_ReadPackets,
	s::CL_ReadPackets.get(),
	void,
	bool final_tick)
{
	if (networking->shouldReadPackets()) {
		CALL_ORIGINAL(final_tick);
	}
}