#pragma once

#include "class_ids.hpp"

enum class ECGroup
{
	PLAYERS_ALL,
	PLAYERS_ENEMIES,
	PLAYERS_TEAMMATES,

	BUILDINGS_ALL,
	BUILDINGS_ENEMIES,
	BUILDINGS_TEAMMATES,
	BUILDINGS_LOCAL,

	PROJECTILES_ALL,
	PROJECTILES_ENEMIES,
	PROJECTILES_TEAMMATES,
	PROJECTILES_LOCAL,

	HEALTH_PACKS,
	AMMO_PACKS,
	HALLOWEEN_GIFTS,
	MVM_MONEY
};

class EntityCache final : public HasLoad, public HasLevelInit, public HasLevelShutdown
{
private:
	std::unordered_map<ECGroup, std::vector<EntityHandle_t>> m_groups{};

private:
	std::unordered_set<int> m_health_pack_indexes{};
	std::unordered_set<int> m_ammo_pack_indexes{};

private:
	C_TFPlayerResource *m_player_resource{};

public:
	bool onLoad() override;
	bool onLevelInit() override;
	bool onLevelShutdown() override;

public:
	C_TFPlayer *const getLocal();
	C_TFWeaponBase *const getWeapon();
	C_TFPlayerResource *const getPlayerResource();
	C_TFGameRulesProxy *const getGameRules();

public:
	void update();

public:
	const std::vector<EntityHandle_t> &getGroup(const ECGroup group);
};

MAKE_UNIQUE(EntityCache, ec);