#pragma once

#include "../../../game/game.hpp"

class MoveSim final
{
private:
	class PlayerDataBackup final
	{
	public:
		vec3 m_vecOrigin{};
		vec3 m_vecVelocity{};
		vec3 m_vecBaseVelocity{};
		vec3 m_vecViewOffset{};
		C_BaseEntity *m_hGroundEntity{};
		int m_fFlags{};
		float m_flDucktime{};
		float m_flDuckJumpTime{};
		bool m_bDucked{};
		bool m_bDucking{};
		bool m_bInDuckJump{};
		float m_flModelScale{};
		float m_flTauntYaw{};
		float m_flCurrentTauntMoveSpeed{};
		int m_iKartState{};
		float m_flVehicleReverseTime{};
		float m_flHypeMeter{};
		float m_flMaxspeed{};
		int m_nAirDucked{};
		bool m_bJumping{};
		int m_iAirDash{};
		vec3 m_vecPunchAngle{};
		vec3 m_vecPunchAngleVel{};
		float m_flJumpTime{};
		unsigned char m_MoveType{};
		unsigned char m_MoveCollide{};
		float m_flFallVelocity{};
		int m_nPlayerCond{};
		int m_nPlayerCondEx{};
		int m_nPlayerCondEx2{};
		int m_nPlayerCondEx3{};
		int m_nPlayerCondEx4{};
		int _condition_bits{};
		CUserCmd *cmd{};
		byte m_nWaterLevel{};

	public:
		void store(C_TFPlayer *const pl);
		void restore(C_TFPlayer *const pl) const;
	};

private:
	PlayerDataBackup m_player_backup{};

private:
	C_TFPlayer *m_player{};
	CMoveData m_mv{};
	CUserCmd m_dummy_user_cmd{};

private:
	bool m_og_in_prediction{};
	bool m_og_first_time_predicted{};
	float m_og_frametime{};

private:
	bool m_sim_running{};

public:
	void setupMoveData(C_TFPlayer* const pl, CMoveData* const move_data, const bool ignore_special_ability);
	bool isRunning() const;
	bool init(C_TFPlayer *const pl, const bool ignore_special_ability = false);
	void restore();
	void tick();
	vec3 origin() const;
};

MAKE_UNIQUE(MoveSim, move_sim);