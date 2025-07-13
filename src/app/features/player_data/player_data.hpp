#pragma once

#include "../../../game/game.hpp"

template <typename T>
class PlayerDataVar final
{
public:
	T cur{};
	T prev{};

public:
	PlayerDataVar &operator=(const T &new_val)
	{
		prev = cur;
		cur = new_val;

		return *this;
	}
};

struct PlayerDataVars final
{
	PlayerDataVar<float> sim_time{};
	PlayerDataVar<int> flags{};
	PlayerDataVar<vec3> origin{};
};

class PlayerData final : public HasLevelShutdown
{
private:
	std::unordered_map<unsigned long, PlayerDataVars> m_data{};

public:
	void postDataUpdate(C_BasePlayer *const pl);
	bool onLevelShutdown() override;
	void update();

public:
	const PlayerDataVars *const get(C_BasePlayer *const pl);
};

MAKE_UNIQUE(PlayerData, player_data);