#include "move_sim.hpp"

#include "../cfg.hpp"
#include "../lag_comp/lag_comp.hpp"
#include "../player_data/player_data.hpp"
#include "../strafe_pred/strafe_pred.hpp"

void MoveSim::PlayerDataBackup::store(C_TFPlayer* const pl)
{
	if (!pl) {
		return;
	}

	m_vecOrigin = pl->m_vecOrigin();
	m_vecVelocity = pl->m_vecVelocity();
	m_vecBaseVelocity = pl->m_vecBaseVelocity();
	m_vecViewOffset = pl->m_vecViewOffset();
	m_hGroundEntity = pl->m_hGroundEntity();
	m_fFlags = pl->m_fFlags();
	m_flDucktime = pl->m_flDucktime();
	m_flDuckJumpTime = pl->m_flDuckJumpTime();
	m_bDucked = pl->m_bDucked();
	m_bDucking = pl->m_bDucking();
	m_bInDuckJump = pl->m_bInDuckJump();
	m_flModelScale = pl->m_flModelScale();
	m_flTauntYaw = pl->m_flTauntYaw();
	m_flCurrentTauntMoveSpeed = pl->m_flCurrentTauntMoveSpeed();
	m_iKartState = pl->m_iKartState();
	m_flVehicleReverseTime = pl->m_flVehicleReverseTime();
	m_flHypeMeter = pl->m_flHypeMeter();
	m_flMaxspeed = pl->m_flMaxspeed();
	m_nAirDucked = pl->m_nAirDucked();
	m_bJumping = pl->m_bJumping();
	m_iAirDash = pl->m_iAirDash();
	m_vecPunchAngle = pl->m_vecPunchAngle();
	m_vecPunchAngleVel = pl->m_vecPunchAngleVel();
	m_flJumpTime = pl->m_flJumpTime();
	m_MoveType = pl->m_MoveType();
	m_MoveCollide = pl->m_MoveCollide();
	m_flFallVelocity = pl->m_flFallVelocity();
	m_nPlayerCond = pl->m_nPlayerCond();
	m_nPlayerCondEx = pl->m_nPlayerCondEx();
	m_nPlayerCondEx2 = pl->m_nPlayerCondEx2();
	m_nPlayerCondEx3 = pl->m_nPlayerCondEx3();
	m_nPlayerCondEx4 = pl->m_nPlayerCondEx4();
	_condition_bits = pl->_condition_bits();
	cmd = pl->m_pCurrentCommand();
	m_nWaterLevel = pl->m_nWaterLevel();
}

void MoveSim::PlayerDataBackup::restore(C_TFPlayer* const pl) const
{
	if (!pl) {
		return;
	}

	pl->m_vecOrigin() = m_vecOrigin;
	pl->m_vecVelocity() = m_vecVelocity;
	pl->m_vecBaseVelocity() = m_vecBaseVelocity;
	pl->m_vecViewOffset() = m_vecViewOffset;
	pl->m_hGroundEntity() = m_hGroundEntity;
	pl->m_fFlags() = m_fFlags;
	pl->m_flDucktime() = m_flDucktime;
	pl->m_flDuckJumpTime() = m_flDuckJumpTime;
	pl->m_bDucked() = m_bDucked;
	pl->m_bDucking() = m_bDucking;
	pl->m_bInDuckJump() = m_bInDuckJump;
	pl->m_flModelScale() = m_flModelScale;
	pl->m_flTauntYaw() = m_flTauntYaw;
	pl->m_flCurrentTauntMoveSpeed() = m_flCurrentTauntMoveSpeed;
	pl->m_iKartState() = m_iKartState;
	pl->m_flVehicleReverseTime() = m_flVehicleReverseTime;
	pl->m_flHypeMeter() = m_flHypeMeter;
	pl->m_flMaxspeed() = m_flMaxspeed;
	pl->m_nAirDucked() = m_nAirDucked;
	pl->m_bJumping() = m_bJumping;
	pl->m_iAirDash() = m_iAirDash;
	pl->m_vecPunchAngle() = m_vecPunchAngle;
	pl->m_vecPunchAngleVel() = m_vecPunchAngleVel;
	pl->m_flJumpTime() = m_flJumpTime;
	pl->m_MoveType() = m_MoveType;
	pl->m_MoveCollide() = m_MoveCollide;
	pl->m_flFallVelocity() = m_flFallVelocity;
	pl->m_nPlayerCond() = m_nPlayerCond;
	pl->m_nPlayerCondEx() = m_nPlayerCondEx;
	pl->m_nPlayerCondEx2() = m_nPlayerCondEx2;
	pl->m_nPlayerCondEx3() = m_nPlayerCondEx3;
	pl->m_nPlayerCondEx4() = m_nPlayerCondEx4;
	pl->_condition_bits() = _condition_bits;
	pl->m_pCurrentCommand() = cmd;
	pl->m_nWaterLevel() = m_nWaterLevel;
}

void MoveSim::setupMoveData(C_TFPlayer* const pl, CMoveData* const move_data, const bool ignore_special_ability)
{
	if (!pl || !move_data) {
		return;
	}

	move_data->m_bFirstRunOfFunctions = false;
	move_data->m_bGameCodeMovedPlayer = false;
	move_data->m_nPlayerHandle = pl->GetRefEHandle().GetIndex();
	move_data->m_vecVelocity = pl->m_vecVelocity();
	move_data->m_vecAbsOrigin = pl->m_vecOrigin();
	move_data->m_flMaxSpeed = pl->TeamFortress_CalculateMaxSpeed(ignore_special_ability);

	if (m_player_backup.m_fFlags & FL_DUCKING) {
		move_data->m_flMaxSpeed *= 0.3333f;
	}

	move_data->m_flClientMaxSpeed = move_data->m_flMaxSpeed;

	math::vectorAngles(move_data->m_vecVelocity, move_data->m_vecViewAngles);

	move_data->m_flForwardMove = 450.0f;
	move_data->m_flSideMove = 0.0f;

	if (pl->m_vecVelocity().length2D() <= move_data->m_flMaxSpeed * 0.1f) {
		move_data->m_flForwardMove = move_data->m_flSideMove = 0.0f;
	}

	move_data->m_vecAngles = move_data->m_vecViewAngles;
	move_data->m_vecOldAngles = move_data->m_vecAngles;

	if (pl->m_hConstraintEntity()) {
		move_data->m_vecConstraintCenter = pl->m_hConstraintEntity()->GetAbsOrigin();
	}

	else {
		move_data->m_vecConstraintCenter = pl->m_vecConstraintCenter();
	}

	move_data->m_flConstraintRadius = pl->m_flConstraintRadius();
	move_data->m_flConstraintWidth = pl->m_flConstraintWidth();
	move_data->m_flConstraintSpeedFactor = pl->m_flConstraintSpeedFactor();
}

bool MoveSim::isRunning() const
{
	return m_sim_running;
}

bool MoveSim::init(C_TFPlayer* const pl, const bool ignore_special_ability)
{
	if (!i::game_movement || !pl || pl->deadflag()) {
		return false;
	}

	m_player = pl;
	m_player_backup.store(m_player);
	m_player->m_pCurrentCommand() = &m_dummy_user_cmd;

	m_og_in_prediction = i::pred->m_bInPrediction;
	m_og_first_time_predicted = i::pred->m_bFirstTimePredicted;
	m_og_frametime = i::global_vars->frametime;

	m_player->m_flDucktime() = 0.0f;
	m_player->m_bDucking() = false;

	if (m_player->m_fFlags() & FL_DUCKING)
	{
		m_player->m_fFlags() &= ~FL_DUCKING;
		m_player->m_bDucked() = true;
		m_player->m_flDuckJumpTime() = 0.0f;
		m_player->m_bInDuckJump() = true;
	}

	m_player->m_flModelScale() -= 0.03125f;

	if (fabsf(m_player->m_vecVelocity().x) < 0.01f) {
		m_player->m_vecVelocity().x = 0.01f;
	}

	if (fabsf(m_player->m_vecVelocity().y) < 0.01f) {
		m_player->m_vecVelocity().y = 0.01f;
	}

	if (!(m_player->m_fFlags() & FL_ONGROUND)) {
		m_player->m_hGroundEntity() = nullptr;
	}

	if ((m_player->m_fFlags() & FL_ONGROUND) || m_player->m_hGroundEntity().Get()) {
		m_player->m_vecOrigin().z += 0.03125f * 3.0f;
		m_player->m_vecVelocity().z = 0.0f;
	}

	setupMoveData(m_player, &m_mv, ignore_special_ability);

	return true;
}

void MoveSim::tick()
{
	if (!i::game_movement || !m_player) {
		return;
	}

	i::pred->m_bInPrediction = true;
	i::pred->m_bFirstTimePredicted = false;
	i::global_vars->frametime = i::pred->m_bEnginePaused ? 0.0f : i::global_vars->interval_per_tick;

	if (m_mv.m_vecVelocity.length() < 15.0f && (m_player->m_fFlags() & FL_ONGROUND)) {
		return;
	}

	m_sim_running = true;
	i::game_movement->ProcessMovement(m_player, &m_mv); //inheritance!!!!
	m_sim_running = false;
}

void MoveSim::restore()
{
	if (!m_player) {
		return;
	}

	m_player_backup.restore(m_player);
	m_player = nullptr;

	i::pred->m_bInPrediction = m_og_in_prediction;
	i::pred->m_bFirstTimePredicted = m_og_first_time_predicted;
	i::global_vars->frametime = m_og_frametime;

	memset(&m_mv, 0, sizeof(CMoveData));
	memset(&m_player_backup, 0, sizeof(PlayerDataBackup));
}

vec3 MoveSim::origin() const
{
	return m_mv.m_vecAbsOrigin;
}