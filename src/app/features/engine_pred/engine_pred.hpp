#pragma once

#include "../../../game/game.hpp"

class EnginePred final
{
private:
	float m_og_curtime{};
	float m_og_frametime{};
	bool m_in_engine_pred{};
public:
	int current_prediction_cmd{};
public:
	void storePredictionResults(int pred_frame) const;
	void restoreEntityToPredictedFrame(int pred_frame) const;
	int getLatestCommandNumber();
public:
	void update();
	bool start(CUserCmd *const cmd, int commands_to_run = 1);
	void end();

public:
	void edgeJump(CUserCmd* const cmd);
	
public:
	bool isInEnginePred() const { return m_in_engine_pred; }
};

MAKE_UNIQUE(EnginePred, engine_pred);

#define RUN_PREDICTION(cmd, func) \
if (engine_pred->start(cmd)) { \
	func(); \
	engine_pred->end(); \
} \