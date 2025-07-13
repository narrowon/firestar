#include "../../../hooks.hpp"

MAKE_HOOK(
	C_BaseAnimating_MaintainSequenceTransitions,
	s::C_BaseAnimating_MaintainSequenceTransitions.get(),
	void,
	void* rcx, void* bone_setup, float cycle, vec3* pos, Vector4D* q)
{
	return;
}