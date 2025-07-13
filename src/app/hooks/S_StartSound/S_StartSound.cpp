#include "../../hooks.hpp"

#include <unordered_map>

using namespace std;

//bomb iraq

class Dormant
{
public:
	struct DormantData
	{
		vec3 location;
		float last_update = 0.f;
	};
	std::unordered_map<int, DormantData> m_mDormancy;
};

MAKE_UNIQUE(Dormant, dormant_data);	

MAKE_HOOK(S_StartSound, s::S_StartSound.get(), int, StartSoundParams_t& params)
{
	if (cfg::esp_faresp && params.soundsource > 0) {
		if (auto* entity = i::ent_list->GetClientEntity(params.soundsource)) {
			if (entity->GetClientClass()->m_ClassID == class_ids::CTFPlayer) {
				// Always update with sound origin regardless of dormancy
				dormant_data->m_mDormancy[params.soundsource] = {
					params.origin,  // Sound position
					i::global_vars->realtime
				};
			}
		}
	}
	return CALL_ORIGINAL(params);
}

MAKE_HOOK(S_StartDynamicSound,
	s::S_StartDynamicSound.get(),
	int, StartSoundParams_t params)
{
	auto update_entry = [&](int index, const Vector& pos, float time)
		{
			if (index >= 0 && index < 32) {
				dormant_data->m_mDormancy[index] = { pos, time };
			}
		};
	auto on_sound = [&](StartSoundParams_t& params)
		{
			if (params.soundsource <= 0 || params.soundsource >= 32)
				return;

			if (auto entity = i::ent_list->GetClientEntity(params.soundsource)) {
				if (auto player = entity->cast<C_TFPlayer>()) {
					if (player->IsDormant() && player->m_iHealth() > 0) {
						player->SetAbsOrigin(params.origin);

						if (player->m_iHealth() == 0)
							player->m_iHealth() = player->GetMaxHealth();

						update_entry(
							params.soundsource,
							params.origin,  // More accurate than entity origin
							i::engine->Time()
						);
					}
				}
			}

		};

	auto manual_network = [&](StartSoundParams_t& params) {
		// Add these checks:
		if (!dormant_data || params.soundsource <= 0) return;
		if (params.soundsource == i::engine->GetLocalPlayer()) return;

		if (auto entity = i::ent_list->GetClientEntity(params.soundsource)) {
			if (auto player = entity->cast<C_TFPlayer>()) {
				if (player && player->IsDormant()) {
					dormant_data->m_mDormancy[params.soundsource] = {
						params.origin,
						i::engine->Time()
					};
				}
			}
		}
		};


	if (cfg::esp_faresp) {
		on_sound(params);
	}

	return CALL_ORIGINAL(params);
}