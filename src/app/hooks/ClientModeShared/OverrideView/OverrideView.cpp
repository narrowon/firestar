#include "../../../hooks.hpp"

MAKE_HOOK(
	ClientModeShared_OverrideView,
	s::ClientModeShared_OverrideView.get(),
	void,
	void* rcx, CViewSetup* pSetup)
{
	if (cfg::tp_override_active && pSetup)
	{
		if (C_TFPlayer* const local{ ec->getLocal() })
		{
			if (input->getKeyInGame(cfg::tp_override_force_toggle_key).pressed && local->m_iObserverMode() == OBS_MODE_NONE) {
				cfg::tp_override_force = !cfg::tp_override_force;
			}

			bool should_tp{ cfg::tp_override_force || local->InCond(TF_COND_TAUNTING) };

			if (local->m_iObserverMode() != OBS_MODE_NONE) {
				should_tp = false;
			}

			static bool should_tp_prev{ should_tp };

			const float cur_time{ i::global_vars->curtime };

			static float lerp_start{};
			static float lerp_end{};

			if (should_tp != should_tp_prev) {
				lerp_start = cur_time;
				lerp_end = cur_time + cfg::tp_override_lerp_time;
				should_tp_prev = should_tp;
			}

			if (should_tp)
			{
				if (local->InFirstPersonView()) {
					i::input->CAM_ToThirdPerson();
					local->ThirdPersonSwitch();
				}

				vec3 forward{};
				vec3 right{};
				vec3 up{};

				math::angleVectors(pSetup->angles, &forward, &right, &up);

				const vec3 offset{ (forward * cfg::tp_override_dist_forward) - (right * cfg::tp_override_dist_right) - (up * cfg::tp_override_dist_up) };

				const float lerp_progress{ math::remap(cur_time, lerp_start, lerp_end, 0.25f, 1.0f) };

				float fraction{ 1.0f };

				if (cfg::tp_override_collision)
				{
					trace_t tr{};

					tf_utils::traceHull
					(
						pSetup->origin, pSetup->origin - (offset * lerp_progress),
						{ -10.0f, -10.0f, -10.0f },
						{ 10.0f, 10.0f, 10.0f },
						&trace_filters::world,
						&tr,
						MASK_SOLID
					);

					fraction = tr.fraction;
				}

				pSetup->origin -= (offset * lerp_progress) * fraction;
			}

			else
			{
				if (!local->InFirstPersonView()) {
					i::input->CAM_ToFirstPerson();
					//local->ThirdPersonSwitch(); //seems to be called in CInput::CAM_ToFirstPerson
				}
			}
		}

		return;
	}

	CALL_ORIGINAL(rcx, pSetup);
}