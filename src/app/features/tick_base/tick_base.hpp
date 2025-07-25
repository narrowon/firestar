#pragma once

#include "../../../game/game.hpp"

class TickShift final
{
public:
	int command_number{};
	int tickbase{};

public:
	TickShift(int sequence_number, int adjusted_tick) : command_number(sequence_number), tickbase(adjusted_tick) {};
};

class TickBase final : public HasLoad, public HasLevelInit
{
public:
	bool m_should_doubletap{};
	int m_processing_ticks{};
	bool m_recharge{};
	bool m_disable_recharge{};
	bool m_should_warp{};
	bool m_should_antiwarp{};
	std::deque<TickShift> m_tick_fixes{};
	std::atomic<int> m_visual_ticks{};
	vec3 m_shift_start_origin{};
	vec3 m_shift_velocity{};

public:
	bool onLoad() override;
	bool onLevelInit() override;

public:
	int getAdjustedTick(const int sim_ticks, const int tickbase, const int server_tick);
	int getShiftLimit(const bool warp = false);
	void sendMoreMove(CUserCmd* const cmd);
	bool rechargeTicks();
	bool canDoubleTap(CUserCmd* const cmd);
	bool checkWarp(CUserCmd* const cmd);
	void updateTickbaseFixes();
	void antiWarp(CUserCmd* const cmd);
	bool shouldAntiWarp() const;
	bool isAttacking(CUserCmd* const cmd, float curtime, const bool vis_indicator = false);
	void addTickShift(const int command_number, const int tickbase);
public:
	void paint();
};

MAKE_UNIQUE(TickBase, tick_base);