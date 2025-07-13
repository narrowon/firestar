#include "seed_pred.h"

#include "../aim_utils/aim_utils.hpp"
#include "../engine_pred/engine_pred.hpp"
#include "../notifs/notifs.hpp"
#include "../cfg.hpp"

bool SeedPred::onLevelShutdown()
{
	m_send_time = 0.0f;
	m_recv_time = 0.0f;
	m_waiting_for_recv = false;
	m_pp_time = 0.0f;

	return true;
}

bool SeedPred::onLoad()
{
	for (int n{}; n <= 255; n++)
	{
		tf_utils::randomSeed(n);
		current_seed = n;

		float x{ tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f) };
		float y{ tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f) };

		m_spread_offsets[n] = { x, y };
	}

	m_spread_init = true;
	return true;
}
double Plat_FloatTime()
{
	static const MemAddr addr{ mem::findExport("tier0.dll", "Plat_FloatTime") };

	if (!addr) {
		return -1.0f;
	}

	return addr.call<double>();
}
void SeedPred::sendPlayerPerf()
{
	if ( m_waiting_for_recv) {
		return;
	}

	if (m_pp_time > 0.0f && (static_cast<float>(Plat_FloatTime()) - m_recv_time) < cfg::seed_pred_resync_interval) {
		return;
	}

	i::client_state->SendStringCmd("playerperf");
	m_send_time = static_cast<float>(Plat_FloatTime());
	m_waiting_for_recv = true;
}

void SeedPred::recvPlayerPerf(bf_read& msg_data)
{
	if (!m_waiting_for_recv) {
		return;
	}

	m_recv_time = static_cast<float>(Plat_FloatTime());
	m_waiting_for_recv = false;

	char msg_str[256]{};

	if (!msg_data.ReadString(msg_str, sizeof(msg_str), true)) {
		return;
	}

	std::string msg{ msg_str };
	msg.erase(msg.begin());

	std::smatch matches{};
	std::regex_match(msg, matches, std::regex{ "(\\d+\\.\\d+) (\\d+) (\\d+) (\\d+\\.\\d+) (\\d+\\.\\d+) vel (\\d+\\.\\d+)" });

	if (matches.size() != 7) {
		return;
	}

	m_pp_time = std::stof(matches[1].str()) + ((m_recv_time - m_send_time) * 1.0f);
}

float SeedPred::predictServerTime() const
{
	if (m_send_time <= 0.0f || m_recv_time <= 0.0f || m_pp_time <= 0.0f) {
		return 0.0f;
	}

	return m_pp_time + (static_cast<float>(Plat_FloatTime()) - m_recv_time);
}

int SeedPred::getSeed(const float time) const
{
	const float seed_time{ time * 1000.0f };

	return *reinterpret_cast<int*>((char*)&seed_time) & 255;
}

void SeedPred::findBestSolution(C_TFPlayer* const pl, CUserCmd* const cmd, const float wpn_spread,
	const bool perfect_shot, const int bullets_per_shot)
{
	// Store original values
	const vec3 origin = pl->m_vecOrigin();
	const CUserCmd backup = *cmd;

	// End current prediction and get seed
	engine_pred->end();
	const int seed = getSeed(predictServerTime());

	// Handle single-bullet weapons (pistols) differently
	if (bullets_per_shot == 1) {
		// Get the spread multiplier for this single bullet
		vec2 multiplier{};
		if (seed > 255) {
			tf_utils::randomSeed(seed);
			multiplier.x = tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f);
			multiplier.y = tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f);
		}
		else {
			multiplier = m_spread_offsets[seed];
		}

		// Calculate spread and adjust angles directly
		const vec3 final_spread{
			multiplier.x * wpn_spread,
			multiplier.y * wpn_spread,
			0.0f
		};

		vec3 forward{}, right{}, up{};
		math::angleVectors(cmd->viewangles, &forward, &right, &up);

		const vec3 fixed = forward + (right * final_spread.x) + (up * final_spread.y);
		vec3 final_angle{};

		math::vectorAngles(fixed, final_angle);
		final_angle = (cmd->viewangles * 2) - final_angle;
		math::clampAngles(final_angle);

		tf_utils::fixMovement(cmd, final_angle);
		cmd->viewangles = final_angle;
	}
	else {
		// Original multi-bullet optimization code
		// Calculate spread multipliers for all bullets
		std::vector<vec2> spread_multipliers;
		vec2 average_multiplier{};

		for (int n = 0; n < bullets_per_shot; n++) {
			// Skip calculation for perfect shots
			if (n == 0 && perfect_shot) {
				spread_multipliers.emplace_back(vec2{});
				continue;
			}

			vec2 multiplier{};
			if (seed + n > 255) {
				tf_utils::randomSeed(seed);
				multiplier.x = tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f);
				multiplier.y = tf_utils::randomFloat(-0.5f, 0.5f) + tf_utils::randomFloat(-0.5f, 0.5f);
			}
			else {
				multiplier = m_spread_offsets[seed + n];
			}

			average_multiplier += multiplier;
			spread_multipliers.emplace_back(multiplier);
		}

		average_multiplier /= bullets_per_shot;

		// Find optimal spread vector using C++20 ranges
		const vec2 final_multiplier = *std::ranges::min_element(
			spread_multipliers,
			[&](const vec2& a, const vec2& b) {
				return a.distTo(average_multiplier) < b.distTo(average_multiplier);
			}
		);

		// Calculate final spread and adjust angles
		const vec3 final_spread{
			final_multiplier.x * wpn_spread,
			final_multiplier.y * wpn_spread,
			0.0f
		};

		vec3 roll_angle = cmd->viewangles;
		roll_angle -= {
			-math::radToDeg(std::atan(final_spread.length2D())),
				0.0f,
				math::radToDeg(std::atan2(final_spread.x, final_spread.y))
		};

		// Apply movement correction
		tf_utils::fixMovement(cmd, roll_angle);
		cmd->viewangles = roll_angle;

		// Validate movement with prediction
		vec3 new_origin{};
		RUN_PREDICTION(cmd, [&]() {
			new_origin = pl->m_vecOrigin();
			});
#define DIST_EPSILON (0.03125)
		// Apply fallback method if movement validation fails
		if (std::fabs(new_origin.x - origin.x) > DIST_EPSILON ||
			std::fabs(new_origin.y - origin.y) > DIST_EPSILON) {

			// Restore original command
			*cmd = backup;

			// Recalculate angles using vector method
			vec3 forward{}, right{}, up{};
			math::angleVectors(cmd->viewangles, &forward, &right, &up);

			const vec3 fixed = forward + (right * final_spread.x) + (up * final_spread.y);
			vec3 final_angle{};

			math::vectorAngles(fixed, final_angle);
			final_angle = (cmd->viewangles * 2) - final_angle;
			math::clampAngles(final_angle);

			tf_utils::fixMovement(cmd, final_angle);
			cmd->viewangles = final_angle;
		}
	}

	// Start prediction for next frame
	if (!engine_pred->start(cmd)) return;
}

auto SeedPred::step() -> float
{
	float t{ m_pp_time * 1000.0f };
	const int i{ *reinterpret_cast<int*>(reinterpret_cast<char*>(&t)) };
	const int e{ (i >> 23) & 0xFF };

	return powf(2.0f, static_cast<float>(e) - (127.0f + 23.0f));
}

void SeedPred::run(CUserCmd* const cmd)
{
	if (!cfg::seed_pred_enable || !cmd || !m_spread_init) {
		return;
	}

	C_TFPlayer* const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return;
	}

	C_TFWeaponBase* const weapon{ ec->getWeapon() };

	if (!weapon || !tf_utils::isWeaponHitscan(weapon)) {
		return;
	}

	const float weapon_spread{ weapon->GetWeaponSpread() };

	if (weapon_spread <= 0.0f) {
		return;
	}

	// Check if weapon is a pistol for more frequent updates
	bool is_pistol = weapon->GetWeaponID() == TF_WEAPON_PISTOL ||
		weapon->GetWeaponID() == TF_WEAPON_PISTOL_SCOUT;

	// Send player performance data more frequently for pistols - logic check for my brain, it makes no sense
	if (is_pistol && (static_cast<float>(Plat_FloatTime()) - m_recv_time) > (cfg::seed_pred_resync_interval * 0.5f)) {
		seed_pred->sendPlayerPerf();
	}
	else {
		seed_pred->sendPlayerPerf();
	}

	const float server_time_pred{ predictServerTime() };

	if (server_time_pred <= 0.0f) {
		return;
	}

	if (!aim_utils->isAttacking(cmd, i::global_vars->curtime)) {
		return;
	}

	CTFWeaponInfo* const weapon_info{ weapon->m_pWeaponInfo() };

	if (!weapon_info) {
		return;
	}

	const int bullets_per_shot{ static_cast<int>(tf_utils::attribHookValue(weapon_info->GetWeaponData(0).m_nBulletsPerShot, "mult_bullets_per_shot", weapon)) };

	if (bullets_per_shot <= 0) {
		return;
	}

	const float last_shot{ i::global_vars->curtime - weapon->m_flLastFireTime() };

	// Modified timing for pistols - reduced perfect shot threshold
	float perfect_shot_threshold = is_pistol ? 0.5f : 1.25f;
	const bool perfect_shot{ (bullets_per_shot > 1 && last_shot > 0.25f) || (bullets_per_shot == 1 && last_shot > perfect_shot_threshold) };

	findBestSolution(local, cmd, weapon_spread, perfect_shot, bullets_per_shot);
}

void SeedPred::paint()
{
	if (!cfg::seed_pred_enable || !cfg::seed_pred_indicator) {
		return;
	}

	C_TFWeaponBase* const weapon{ ec->getWeapon() };

	if (!weapon || !tf_utils::isWeaponHitscan(weapon)) {
		return;
	}

	const std::chrono::hh_mm_ss time{ std::chrono::seconds(static_cast<int>(predictServerTime())) };
	const int total_mins{ time.hours().count() * 60 + time.minutes().count() };


	if (cfg::seed_pred_indicator)
	{
		// Text content
		const std::string step_str = std::format("step: {:.0f}, h{}", step(), time.hours().count());

		// Position calculations
		const vec2 base_pos{
			draw->getScreenSize().x * cfg::seed_pred_indicator_pos_x,
			draw->getScreenSize().y * cfg::seed_pred_indicator_pos_y
		};

		// Rectangle dimensions
		constexpr float padding = 10.0f;
		constexpr float rect_width = 100.0f;
		constexpr float rect_height = 35.0f;

		// Text positioning
		const vec2 step_pos{
			base_pos.x,
			base_pos.y - rect_height / 4  // Higher position for step
		};

		const vec2 time_pos{
			base_pos.x,
			base_pos.y + rect_height / 4 - 2.0f  // Move up by 2 pixels
		};

		draw->string(fonts::font_indicators, step_pos, step_str, Color(255, 255, 255, 255), POS_CENTERXY);
	}
}
