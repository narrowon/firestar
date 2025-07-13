#include "../../../hooks.hpp"


MAKE_HOOK(
	C_TFViewModel_CalcViewModelView,
	s::C_TFViewModel_CalcViewModelView.get(),
	void,
	void* rcx, C_BasePlayer* owner, const Vector& eyePosition, const QAngle& eyeAngles)
{
	if (!cfg::vm_override_active) {
		CALL_ORIGINAL(rcx, owner, eyePosition, eyeAngles);
		return;
	}

	static ConVar* const tf_viewmodels_offset_override{ i::cvar->FindVar("tf_viewmodels_offset_override") };
	static ConVar* const cl_wpn_sway_interp{ i::cvar->FindVar("cl_wpn_sway_interp") };

	if (!tf_viewmodels_offset_override || !cl_wpn_sway_interp) {
		CALL_ORIGINAL(rcx, owner, eyePosition, eyeAngles);
		return;
	}

	const std::string og_tf_viewmodels_offset_override{ tf_viewmodels_offset_override->GetString() };
	const float og_cl_wpn_sway_interp{ cl_wpn_sway_interp->GetFloat() };

	tf_viewmodels_offset_override->SetValue(std::format("{} {} {}", cfg::vm_override_offset_x, cfg::vm_override_offset_y, cfg::vm_override_offset_z).c_str());
	cl_wpn_sway_interp->SetValue(math::remap(cfg::vm_override_sway, 0.0f, 100.0f, 0.0f, 0.1f));

	CALL_ORIGINAL(rcx, owner, eyePosition, eyeAngles);

	tf_viewmodels_offset_override->SetValue(og_tf_viewmodels_offset_override.c_str());
	cl_wpn_sway_interp->SetValue(og_cl_wpn_sway_interp);
}