#include "player_data.hpp"

void PlayerData::postDataUpdate(C_BasePlayer *const pl)
{
	if (!pl) {
		return;
	}

	PlayerDataVars &pd{ m_data[pl->GetRefEHandle().GetIndex()] };

	pd.sim_time = pl->m_flSimulationTime();
	pd.flags = pl->m_fFlags();
	pd.origin = pl->m_vecOrigin();
}

bool PlayerData::onLevelShutdown()
{
	m_data.clear();

	return true;
}

void PlayerData::update()
{
	for (const std::pair<unsigned long, PlayerDataVars> &d : m_data)
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(d.first) };

		if (!ent || ent->IsDormant()) {
			m_data.erase(d.first);
		}
	}
}

const PlayerDataVars *const PlayerData::get(C_BasePlayer *const pl)
{
	if (!pl) {
		return nullptr;
	}

	const unsigned long index{ pl->GetRefEHandle().GetIndex() };

	if (!m_data.contains(index)) {
		return nullptr;
	}

	return &m_data[index];
}