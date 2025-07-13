#include "engine_pred.hpp"

#include "../cfg.hpp"

void EnginePred::update()
{
	i::pred->Update
	(
		i::client_state->m_nDeltaTick,
		i::client_state->m_nDeltaTick > 0,
		i::client_state->last_command_ack,
		getLatestCommandNumber() - 1
	);
}

bool EnginePred::start(CUserCmd *const cmd, int commands_to_run)
{
	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag() || !cmd) {
		return false;
	}

	//if we don't save the data manually before restoring it hit fx gets suppressed? no idea why
	storePredictionResults(getLatestCommandNumber() - 1);

	m_og_curtime = i::global_vars->curtime;
	m_og_frametime = i::global_vars->frametime;

	const bool og_in_prediction{ i::pred->m_bInPrediction };
	const bool og_first_time_predicted{ i::pred->m_bFirstTimePredicted };

	i::pred->m_bInPrediction = true;
	i::pred->m_bFirstTimePredicted = false;

	//disable all calls to post think since we only want to predict up to our bullet firing which the call to post think will screw up
	m_in_engine_pred = true;

	//calling runcommand is better than rebuilding it as theres only two things that dont need to be called for engine pred
	//plus weapon selection rtti is a bitch and i rather not rebuild that
	i::move_helper->SetHost(local);
	i::pred->RunCommand(local, cmd, i::move_helper);
	i::move_helper->SetHost(NULL);

	m_in_engine_pred = false;

	i::pred->m_bInPrediction = og_in_prediction;
	i::pred->m_bFirstTimePredicted = og_first_time_predicted;

	return true;
}

void EnginePred::end()
{
	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return;
	}

	i::global_vars->curtime = m_og_curtime;
	i::global_vars->frametime = m_og_frametime;

	//restore all entity states to the last predicted cmd
	restoreEntityToPredictedFrame(getLatestCommandNumber() - 1);
}

void EnginePred::storePredictionResults(int pred_frame) const
{
	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	static ConVar *const cl_predict = i::cvar->FindVar("cl_predict");

	if (!cl_predict || cl_predict->GetInt() == 0) {
		return;
	}

	//YES theres a list full with the entities that are predictables
	//NO i dont care
	for (int iter{ 1 }; iter <= i::ent_list->GetHighestEntityIndex(); iter++)
	{
		C_BaseEntity *const ent{ i::ent_list->GetClientEntity(iter)->cast<C_BaseEntity>() };

		if (!ent || !ent->m_bPredictable()) {
			continue;
		}

		ent->SaveData("save_engine_pred", pred_frame, PC_EVERYTHING);
	}
}

void EnginePred::restoreEntityToPredictedFrame(int pred_frame) const
{
	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	static ConVar *const cl_predict = i::cvar->FindVar("cl_predict");

	if (!cl_predict || cl_predict->GetInt() == 0) {
		return;
	}

	//YES theres a list full with the entities that are predictables
	//NO i dont care
	for (int iter{ 1 }; iter <= i::ent_list->GetHighestEntityIndex(); iter++)
	{
		C_BaseEntity *const ent{ i::ent_list->GetClientEntity(iter)->cast<C_BaseEntity>() };

		if (!ent || !ent->m_bPredictable()) {
			continue;
		}

		ent->RestoreData("restore_engine_pred", pred_frame, PC_EVERYTHING);
	}
}

int EnginePred::getLatestCommandNumber()
{
	return i::client_state->lastoutgoingcommand + i::client_state->chokedcommands + 1;
}

void EnginePred::edgeJump(CUserCmd *const cmd)
{
	if (!cfg::auto_edge_jump_active || !input_tick->getKeyInGame(cfg::auto_edge_jump_key).held) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag() || !cmd) {
		return;
	}

	//if we are going to jump anyways return outta this
	if (cmd->buttons & IN_JUMP) {
		return;
	}

	// need to be on the ground to be able to use this!
	if (!(local->m_fFlags() & FL_ONGROUND)) {
		return;
	}

	if (start(cmd))
	{
		if (!(local->m_fFlags() & FL_ONGROUND)) { //predicted our command to be not on ground anymore
			cmd->buttons |= IN_JUMP;
		}

		end();
	}
}