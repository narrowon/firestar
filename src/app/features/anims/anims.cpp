#include "anims.hpp"

#include "../player_data/player_data.hpp"
#include "../cfg.hpp"

void Anims::postDataUpdate(C_BasePlayer *const base_pl)
{
	if (!base_pl
		|| base_pl->GetClassId() != class_ids::CTFPlayer
		|| base_pl->m_nSequence() == -1
		|| base_pl->entindex() == i::engine->GetLocalPlayer()) {
		return;
	}

	const PlayerDataVars *const pl_data{ player_data->get(base_pl) };

	if (!pl_data) {
		return;
	}

	C_TFPlayer *const pl{ base_pl->cast<C_TFPlayer>() };

	if (!pl || pl->InCond(TF_COND_TAUNTING)) {
		return;
	}

	CTFPlayerAnimState *const anim_state{ pl->m_PlayerAnimState() };

	if (!anim_state) {
		return;
	}

	const float og_curtime{ i::global_vars->curtime };
	const float og_frametime{ i::global_vars->frametime };

	i::global_vars->curtime = pl_data->sim_time.prev;
	i::global_vars->frametime = i::global_vars->interval_per_tick;

	const int update_ticks{ std::clamp(tf_utils::timeToTicks(pl_data->sim_time.cur - pl_data->sim_time.prev), 1, 24) };

	for (int n{}; n < update_ticks; n++) {
		anim_state->Update(pl->m_angEyeAngles().y, pl->m_angEyeAngles().x);
		pl->FrameAdvance(0.0f);
		i::global_vars->curtime += i::global_vars->interval_per_tick; //set any animation related timers in the right state
	}

	i::global_vars->curtime = og_curtime;
	i::global_vars->frametime = og_frametime;

	const vec3 abs_angles{ pl->GetAbsAngles() };

	pl->SetAbsAngles({ abs_angles.x, anim_state->m_flCurrentFeetYaw, abs_angles.z });
	pl->SetAbsOrigin(pl->m_vecOrigin());
}