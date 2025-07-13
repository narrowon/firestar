#include "../../../hooks.hpp"


MAKE_HOOK(
	C_BaseEntity_SetAbsVelocity,
	s::C_BaseEntity_SetAbsVelocity.get(),
	void,
	void* rcx, const Vector& vecAbsVelocity)
{
	if (reinterpret_cast<uintptr_t>(_ReturnAddress()) != s::C_BaseEntity_SetAbsVelocity_C_BasePlayer_PostDataUpdate_call.get()) {
		CALL_ORIGINAL(rcx, vecAbsVelocity);
		return;
	}

	C_BasePlayer* const base_pl{ reinterpret_cast<C_BasePlayer*>(rcx) };

	if (!base_pl || base_pl->GetClassId() != class_ids::CTFPlayer) {
		CALL_ORIGINAL(rcx, vecAbsVelocity);
		return;
	}

	C_TFPlayer* const pl{ base_pl->cast<C_TFPlayer>() };

	if (!pl || pl->deadflag()) {
		CALL_ORIGINAL(rcx, vecAbsVelocity);
		return;
	}

	const PlayerDataVars* const pd{ player_data->get(pl) };

	if (!pd || (pd->sim_time.cur - pd->sim_time.prev) <= 0.0f || (pd->flags.cur & FL_ONGROUND)) {
		CALL_ORIGINAL(rcx, vecAbsVelocity);
		return;
	}

	vec3 prev_origin{ pd->origin.prev };
	bool fix_applied{};

	if ((pd->flags.cur & FL_DUCKING) && !(pd->flags.prev & FL_DUCKING)) {
		prev_origin.z += 20.0f;
		fix_applied = true;
	}

	if (!(pd->flags.cur & FL_DUCKING) && (pd->flags.prev & FL_DUCKING)) {
		prev_origin.z -= 20.0f;
		fix_applied = true;
	}

	if (!fix_applied) {
		CALL_ORIGINAL(rcx, vecAbsVelocity);
		return;
	}

	CALL_ORIGINAL(rcx, { vecAbsVelocity.x, vecAbsVelocity.y, (pd->origin.cur.z - prev_origin.z) / (pd->sim_time.cur - pd->sim_time.prev) });
}