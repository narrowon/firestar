#include "../../../hooks.hpp"


MAKE_HOOK(
	CL_Move,
	s::CL_Move.get(),
	void,
	float accumulated_extra_samples, bool final_tick)
{
	networking->fixNetworking(final_tick);

	networking->move(accumulated_extra_samples, final_tick);
}