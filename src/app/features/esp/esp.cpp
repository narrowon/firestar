#include "esp.hpp"

#include "../cfg.hpp"
#include "../visual_utils/visual_utils.hpp"

void ESP::drawName(const float x, const float y, const float w, const float h, std::string_view name, const Color clr, size_t *text_offset)
{
	vec2 pos{ x + (w / 2), (y - (fonts::font_esp->FontSize + cfg::esp_spacing_y)) };

	if (cfg::esp_name_pos == 1 && text_offset) {
		pos = vec2{ x + (w + cfg::esp_spacing_x), y + (fonts::font_esp->FontSize * (*text_offset)) };
		*text_offset += 1;
	}

	draw->string(fonts::font_esp, pos, name, clr, cfg::esp_name_pos == 0 ? POS_CENTERX : POS_DEFAULT);
	//draw->string(pos, fonts::esp.get(), name, clr, cfg::esp_name_pos == 0 ? POS_CENTERX : POS_DEFAULT);
}

void ESP::drawHpBar(const float x, const float y, const float w, const float h, float hp, const float max_hp, const Color clr, const bool outline)
{
	if (hp > max_hp) {
		hp = max_hp;
	}

	const float bar_w{ 2.0f };
	const float bar_h{ h };

	const float bar_x{ x - ((bar_w * 2.0f) + cfg::esp_spacing_x) };
	const float bar_y{ y };

	const float fill_h{ std::floorf((hp / max_hp) * bar_h) };

	if (outline) {
		draw->rectFilled({ bar_x - 1.0f, bar_y - 1.0f }, { bar_w + 2.0f, bar_h + 2.0f }, cfg::color_esp_outline);
	}

	draw->rectFilled({ bar_x, bar_y + (bar_h - fill_h) }, { bar_w, fill_h }, clr);

	if (cfg::esp_healthbar_divisions > 1)
	{
		for (int n{ 1 }; n < cfg::esp_healthbar_divisions; n++) {
			draw->rectFilled({ bar_x, bar_y + ((bar_h / cfg::esp_healthbar_divisions) * n) }, { bar_w, 1.0f }, cfg::color_esp_outline);
			draw->rectFilled({ bar_x, bar_y + ((bar_h / cfg::esp_healthbar_divisions) * n) }, { bar_w, 1.0f }, cfg::color_esp_outline);
		}
	}
}

void ESP::drawBox(const float x, const float y, const float w, const float h, const Color clr)
{
	draw->rect({ x, y }, { w, h }, clr);
	draw->rect({ x - 1, y - 1 }, { w + 2, h + 2 }, cfg::color_esp_outline);
	draw->rect({ x + 1, y + 1 }, { w - 2, h - 2 }, cfg::color_esp_outline);
}
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
void ESP::run()
{
	if (!cfg::esp_active || !vis_utils->shouldRunVisuals()) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local) {
		return;
	}

	//const float original_alpha{ i::surface->DrawGetAlphaMultiplier() };

	if (cfg::esp_players_active)
	{
		//i::surface->DrawSetAlphaMultiplier(cfg::esp_players_alpha);
		

		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PLAYERS_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent) {
				continue;
			}

			C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

			if (!pl || pl->deadflag()) {
				continue;
			}

			if (vis_utils->shouldSkipPlayer(
				local,
				pl,
				cfg::esp_players_ignore_local,
				cfg::esp_players_ignore_friends,
				cfg::esp_players_ignore_enemies,
				cfg::esp_players_ignore_teammates))
			{
				continue;
			}

			float x{};
			float y{};
			float w{};
			float h{};

			if (!vis_utils->getScreenBounds(pl, x, y, w, h)) {
				continue;
			}

			const float hp{ static_cast<float>(pl->m_iHealth()) };
			const float max_hp{ static_cast<float>(pl->GetMaxHealth()) };

			const Color clr{ vis_utils->getEntColor(pl) };
			const Color hp_clr{ vis_utils->getHpColor(hp, max_hp) };
			const Color text_clr{ cfg::esp_text_color == 0 ? clr : cfg::color_esp_text };

			size_t text_offset{};
			const float currentTime = i::engine->Time();
			for (auto it = dormant_data->m_mDormancy.begin(); it != dormant_data->m_mDormancy.end();) {
				if (currentTime - it->second.last_update > 5.0f) {
					it = dormant_data->m_mDormancy.erase(it);
				}
				else {
					++it;
				}
			}

			if (cfg::esp_faresp) {
				for (int e{}; e < i::ent_list->GetHighestEntityIndex(); e++) {
					IClientEntity* const client_ent{ i::ent_list->GetClientEntity(e) };

					if (!client_ent || client_ent->GetClientClass()->m_ClassID != class_ids::CTFPlayer)
						continue;

					C_BaseEntity* const base_ent{ client_ent->GetBaseEntity() };
					C_TFPlayer* player = base_ent->cast<C_TFPlayer>();

					if (!player ||
						player->m_iHealth() <= 0 ||
						player->m_iTeamNum() == ec->getLocal()->m_iTeamNum() ||
						player == ec->getLocal())
					{
						if (dormant_data->m_mDormancy.contains(e)) {
							dormant_data->m_mDormancy.erase(e);
						}
						continue;
					}

					if (!dormant_data->m_mDormancy.contains(e))
						continue;

					// Get stored sound origin
					const auto& sound_data = dormant_data->m_mDormancy[e];
					const Vector& soundOrigin = sound_data.location;

					// Convert world coordinates to screen coordinates
					float x{}, y{}, w{}, h{};
					if (!vis_utils->getScreenBounds(player, x, y, w, h)) {
						continue;
					}

					// Distance check from LOCAL player to SOUND ORIGIN
					const float distance = (soundOrigin - ec->getLocal()->m_vecOrigin()).length();
					if (distance <= cfg::esp_faresp_distance) {
						drawBox(x, y, w, h, Color(255, 255, 255, 255));
					}
				}
			}

			if (cfg::esp_players_name)
			{
				player_info_t pi{};

				if (i::engine->GetPlayerInfo(pl->entindex(), &pi)) {
					drawName(x, y, w, h, pi.name, text_clr, &text_offset);
				}
			}

			if (cfg::esp_players_class)
			{
				draw->string
				(
					fonts::font_esp,
					{ (x + w) + cfg::esp_spacing_x, y + (fonts::font_esp->FontSize * text_offset++) },
					vis_utils->getClassName(pl->m_iClass()),
					text_clr,
					POS_DEFAULT
				);
			}

			if (cfg::esp_players_health)
			{
				draw->string
				(
					fonts::font_esp,
					{ (x + w) + cfg::esp_spacing_x, y + (fonts::font_esp->FontSize * text_offset++) },
					std::to_string(static_cast<int>(hp)),
					hp > max_hp ? cfg::color_overheal : hp_clr,
					POS_DEFAULT
				);
			}

			if (cfg::esp_players_health_bar)
			{
				drawHpBar(x, y, w, h, hp, max_hp, hp_clr);

				if (hp > max_hp) {
					drawHpBar(x, y, w, h, hp - max_hp, max_hp * 0.48f, cfg::color_overheal, false);
				}
			}

			if (cfg::esp_players_uber)
			{
				if (pl->m_iClass() == TF_CLASS_MEDIC)
				{
					if (C_TFWeaponBase *const wep{ pl->GetWeapon(1) })
					{
						if (wep->GetWeaponID() == TF_WEAPON_MEDIGUN)
						{
							if (C_WeaponMedigun *const medgun{ wep->cast<C_WeaponMedigun>() })
							{
								std::string uber_str{};

								if (medgun->GetMedigunType() == MEDIGUN_RESIST) {
									uber_str = std::format("{}/4", static_cast<int>(medgun->m_flChargeLevel() * 4.0f));
								}

								else {
									uber_str = std::format("{:.0f}%", medgun->m_flChargeLevel() * 100.0f);
								}

								draw->string
								(
									fonts::font_esp,
									{ (x + w) + cfg::esp_spacing_x, y + (fonts::font_esp->FontSize * text_offset++) },
									uber_str,
									cfg::color_uber,
									POS_DEFAULT
								);
							}
						}
					}
				}
			}

			if (cfg::esp_players_uber_bar)
			{
				if (pl->m_iClass() == TF_CLASS_MEDIC)
				{
					if (C_TFWeaponBase *const wep{ pl->GetWeapon(1) })
					{
						if (wep->GetWeaponID() == TF_WEAPON_MEDIGUN)
						{
							if (C_WeaponMedigun *const medgun{ wep->cast<C_WeaponMedigun>() })
							{
								const float charge{ medgun->m_flChargeLevel() };
								const float max_charge{ 1.0f };

								if (medgun->GetMedigunType() == MEDIGUN_RESIST)
								{
									const int num_bars{ 4 };
									const float bar_h{ 2.0f };
									const float spacing{ 4.0f };

									const float bar_w{ w / 2.0f - spacing / 2.0f };

									const float total_spacing{ (num_bars - 1) * spacing };
									const float bar_x{ x };

									float bar_y{ y + h + (cfg::esp_spacing_y + 2.0f) };

									for (int n{}; n < num_bars; ++n)
									{
										const float segment_charge_start{ n / static_cast<float>(num_bars) };
										const float segment_charge_end{ (n + 1) / static_cast<float>(num_bars) };

										float fill_w{};

										if (charge >= segment_charge_end) {
											fill_w = bar_w;
										}

										else if (charge > segment_charge_start) {
											fill_w = std::floorf((charge - segment_charge_start) / (segment_charge_end - segment_charge_start) * bar_w);
										}

										const float segment_x{ bar_x + (n % 2) * (bar_w + spacing) };

										if (n % 2 == 0 && n > 0) {
											bar_y += bar_h + (cfg::esp_spacing_y + 2.0f);
										}

										draw->rectFilled
										(
											{ segment_x - 1.0f, bar_y - 1.0f },
											{ bar_w + 2.0f, bar_h + 2.0f },
											cfg::color_esp_outline
										);

										draw->rectFilled
										(
											{ segment_x, bar_y },
											{ fill_w, bar_h },
											cfg::color_uber
										);
									}
								}

								else
								{
									const float bar_h{ 2.0f };
									const float bar_w{ w };

									const float bar_x{ x };
									const float bar_y{ y + h + (cfg::esp_spacing_y + 2.0f) };

									const float fill_w{ std::floorf((charge / max_charge) * bar_w) };

									draw->rectFilled({ bar_x - 1.0f, bar_y - 1.0f }, { bar_w + 2.0f, bar_h + 2.0f }, cfg::color_esp_outline);
									draw->rectFilled({ bar_x, bar_y }, { fill_w, bar_h }, cfg::color_uber);
								}
							}
						}
					}
				}
			}

			if (cfg::esp_players_box)
			{
				drawBox(x, y, w, h, clr);
			}

			if (cfg::esp_players_conds)
			{
				if (text_offset > 0) {
					text_offset++;
				}

				const float draw_x{ (x + w) + cfg::esp_spacing_x };
				const float tall{ fonts::font_esp->FontSize };

				if (pl->IsZoomed()) {
					draw->string(fonts::font_esp,{ draw_x, y + (tall * text_offset++) }, "ZOOM", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->IsInvisible()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "INVIS", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->m_bFeignDeathReady()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "DR", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->IsInvulnerable()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "INVULN", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->IsCritBoosted()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "CRIT", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->IsMiniCritBoosted()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "MINICRIT", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->IsMarked()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "MARKED", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_MAD_MILK)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "MILK", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_TAUNTING)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "TAUNT", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_DISGUISED)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "DISGUISE", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_BURNING) || pl->InCond(TF_COND_BURNING_PYRO)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BURNING", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_OFFENSEBUFF)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BANNER", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_DEFENSEBUFF)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BACKUP", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_REGENONDAMAGEBUFF)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "CONCH", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_MEDIGUN_UBER_BULLET_RESIST)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BULLETRES", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_MEDIGUN_UBER_BLAST_RESIST)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BLASTRES", cfg::color_conds, POS_DEFAULT);
				}

				if (pl->InCond(TF_COND_MEDIGUN_UBER_FIRE_RESIST)) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "FIRERES", cfg::color_conds, POS_DEFAULT);
				}
			}
		}
	}

	if (cfg::esp_buildings_active)
	{

		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::BUILDINGS_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent) {
				continue;
			}

			C_BaseObject *const obj{ ent->cast<C_BaseObject>() };

			if (!obj || obj->m_iHealth() <= 0) {
				continue;
			}

			if (vis_utils->shouldSkipBuilding(
				local,
				obj,
				cfg::esp_buildings_ignore_local,
				cfg::esp_buildings_ignore_enemies,
				cfg::esp_buildings_ignore_teammates))
			{
				continue;
			}

			float x{};
			float y{};
			float w{};
			float h{};

			if (!vis_utils->getScreenBounds(obj, x, y, w, h)) {
				continue;
			}

			const float hp{ static_cast<float>(obj->m_iHealth()) };
			const float max_hp{ static_cast<float>(obj->m_iMaxHealth()) };

			const Color clr{ vis_utils->getEntColor(obj) };
			const Color hp_clr{ vis_utils->getHpColor(hp, max_hp) };
			const Color text_clr{ cfg::esp_text_color == 0 ? clr : cfg::color_esp_text };

			size_t text_offset{};

			if (cfg::esp_buildings_name)
			{
				std::string name{ "building" };

				if (obj->GetClassId() == class_ids::CObjectSentrygun)
				{
					if (obj->m_bMiniBuilding()) {
						name = "mini sentry";
					}

					else {
						name = "sentry";
					}
				}

				else if (obj->GetClassId() == class_ids::CObjectDispenser) {
					name = "dispenser";
				}

				else if (obj->GetClassId() == class_ids::CObjectTeleporter)
				{
					if (obj->m_iObjectMode() == MODE_TELEPORTER_ENTRANCE) {
						name = "tele in";
					}

					else {
						name = "tele out";
					}
				}

				drawName(x, y, w, h, obj->m_bMiniBuilding() ? name : std::format("{} ({})", name, obj->m_iUpgradeLevel()), text_clr, &text_offset);
			}

			if (cfg::esp_buildings_health)
			{
				draw->string
				(
					fonts::font_esp,
					{ (x + w) + cfg::esp_spacing_x, y + (fonts::font_esp->FontSize * text_offset++) },
					std::to_string(static_cast<int>(hp)),
					hp_clr,
					POS_DEFAULT
				);
			}

			if (cfg::esp_buildings_health_bar)
			{
				drawHpBar(x, y, w, h, hp, max_hp, hp_clr);
			}

			if (cfg::esp_buildings_box)
			{
				drawBox(x, y, w, h, clr);
			}

			if (cfg::esp_buildings_conds)
			{
				if (text_offset > 0) {
					text_offset++;
				}

				const float draw_x{ (x + w) + cfg::esp_spacing_x };
				const float tall{ fonts::font_esp->FontSize };

				if (obj->IsDisabled()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "DISABLED", cfg::color_conds, POS_DEFAULT);
				}

				if (obj->m_bBuilding()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "BUILDING", cfg::color_conds, POS_DEFAULT);
				}

				if (obj->GetClassId() == class_ids::CObjectSentrygun && obj->cast<C_ObjectSentrygun>()->m_bShielded()) {
					draw->string(fonts::font_esp, { draw_x, y + (tall * text_offset++) },  "WRANGLED", cfg::color_conds, POS_DEFAULT);
				}
			}
		}
	}

	if (cfg::esp_projectiles_active)
	{

		for (const EntityHandle_t ehandle : ec->getGroup(ECGroup::PROJECTILES_ALL))
		{
			IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

			if (!ent || !ent->ShouldDraw()) {
				continue;
			}

			C_BaseEntity *const proj{ ent->cast<C_BaseEntity>() };

			if (!proj) {
				continue;
			}

			if (vis_utils->shouldSkipProjectile(
				local,
				proj,
				cfg::esp_projectiles_ignore_local,
				cfg::esp_projectiles_ignore_enemies,
				cfg::esp_projectiles_ignore_teammates))
			{
				continue;
			}

			float x{};
			float y{};
			float w{};
			float h{};

			if (!vis_utils->getScreenBounds(proj, x, y, w, h)) {
				continue;
			}

			const Color clr{ vis_utils->getEntColor(proj) };
			const Color text_clr{ cfg::esp_text_color == 0 ? clr : cfg::color_esp_text };

			if (cfg::esp_projectiles_name)
			{
				std::string name{ "projectile" };

				const size_t class_id{ ent->GetClassId() };

				if (class_id == class_ids::CTFProjectile_Rocket || class_id == class_ids::CTFProjectile_SentryRocket) {
					name = "rocket";
				}

				else if (class_id == class_ids::CTFProjectile_Jar) {
					name = "jarate";
				}

				else if (class_id == class_ids::CTFProjectile_JarGas) {
					name = "gas";
				}

				else if (class_id == class_ids::CTFProjectile_JarMilk) {
					name = "milk";
				}

				else if (class_id == class_ids::CTFProjectile_Arrow) {
					name = "arrow";
				}

				else if (class_id == class_ids::CTFProjectile_Flare) {
					name = "flare";
				}

				else if (class_id == class_ids::CTFProjectile_Cleaver) {
					name = "cleaver";
				}

				else if (class_id == class_ids::CTFProjectile_HealingBolt) {
					name = "healing bolt";
				}

				else if (class_id == class_ids::CTFGrenadePipebombProjectile)
				{
					C_TFGrenadePipebombProjectile *const pipebomb{ ent->cast<C_TFGrenadePipebombProjectile>() };

					if (pipebomb->HasStickyEffects()) {
						name = "sticky";
					}

					else if (pipebomb->m_iType() == TF_GL_MODE_CANNONBALL) {
						name = "cannonball";
					}

					else {
						name = "pipe";
					}
				}

				drawName(x, y, w, h, name, text_clr, nullptr);
			}

			if (cfg::esp_projectiles_box)
			{
				drawBox(x, y, w, h, clr);
			}
		}
	}

	if (cfg::esp_world_active)
	{
		//i::surface->DrawSetAlphaMultiplier(cfg::esp_world_alpha);

		auto drawGroup = [&](const ECGroup group, const Color clr, std::string_view name)
		{
			for (const EntityHandle_t ehandle : ec->getGroup(group))
			{
				IClientEntity *const client_ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

				if (!client_ent || !client_ent->ShouldDraw()) {
					continue;
				}

				C_BaseEntity *const ent{ client_ent->cast<C_BaseEntity>() };

				if (!ent) {
					continue;
				}

				float x{};
				float y{};
				float w{};
				float h{};

				if (!vis_utils->getScreenBounds(ent, x, y, w, h)) {
					continue;
				}

				const Color text_clr{ cfg::esp_text_color == 0 ? clr : cfg::color_esp_text };

				if (cfg::esp_world_name)
				{
					drawName(x, y, w, h, name, text_clr, nullptr);
				}

				if (cfg::esp_world_box)
				{
					drawBox(x, y, w, h, clr);
				}
			}
		};

		if (!cfg::esp_world_ignore_health_packs) {
			drawGroup(ECGroup::HEALTH_PACKS, cfg::color_health_pack, "health");
		}

		if (!cfg::esp_world_ignore_ammo_packs) {
			drawGroup(ECGroup::AMMO_PACKS, cfg::color_ammo_pack, "ammo");
		}

		if (!cfg::esp_world_ignore_halloween_gifts) {
			drawGroup(ECGroup::HALLOWEEN_GIFTS, cfg::color_halloween_gift, "gargoyle");
		}

		if (!cfg::esp_world_ignore_mvm_money) {
			drawGroup(ECGroup::MVM_MONEY, cfg::color_mvm_money, "money");
		}
	}

	//i::surface->DrawSetAlphaMultiplier(original_alpha);
}