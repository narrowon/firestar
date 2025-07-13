#pragma once

#include "../../shared/shared.hpp"

#pragma region aimbot

MAKE_CFGVAR(aimbot_active, true);
MAKE_CFGVAR(aimbot_mode, 0); //(0 = on key) (1 = on attack)
MAKE_CFGVAR(aimbot_key, VK_SHIFT);
MAKE_CFGVAR(aimbot_key_style, 2);
MAKE_CFGVAR(aimbot_auto_shoot, true);

MAKE_CFGVAR(aimbot_ignore_friends, true);
MAKE_CFGVAR(aimbot_ignore_invisible, false);
MAKE_CFGVAR(aimbot_ignore_invulnerable, true);
MAKE_CFGVAR(aimbot_ignore_buildings, true);

MAKE_CFGVAR(aimbot_hitscan_active, true);
MAKE_CFGVAR(aimbot_hitscan_mode, 0); //(0 = fov) (1 = distance)
MAKE_CFGVAR(aimbot_hitscan_aim_method, 1); //(0 = normal) (1 = smooth) (2 = silent) (3 = psilent)
MAKE_CFGVAR(aimbot_hitscan_aim_pos, 0); //(0 = auto) (1 = body) (2 = head)
MAKE_CFGVAR(aimbot_hitscan_fov, 20.0f);
MAKE_CFGVAR(aimbot_hitscan_smooth, 25.0f);
MAKE_CFGVAR(aimbot_hitscan_hitchance, 0.0f);
MAKE_CFGVAR(aimbot_hitscan_wait_for_headshot, true);
MAKE_CFGVAR(aimbot_hitscan_bbox_multipoint, true);
//MAKE_CFGVAR(aimbot_hitscan_aim_all_hitboxes, true); this can be auto
MAKE_CFGVAR(aimbot_hitscan_target_stickies, true);

MAKE_CFGVAR(aimbot_melee_active, true);
MAKE_CFGVAR(aimbot_melee_mode, 1); //(0 = fov) (1 = distance)
MAKE_CFGVAR(aimbot_melee_aim_method, 1); //(0 = normal) (1 = smooth) (2 = silent) (3 = psilent)
MAKE_CFGVAR(aimbot_melee_fov, 60.0f);
MAKE_CFGVAR(aimbot_melee_smooth, 25.0f);
MAKE_CFGVAR(aimbot_melee_swing_pred, true);
MAKE_CFGVAR(aimbot_melee_swing_pred_time, 0.2f);

MAKE_CFGVAR(aimbot_projectile_active, true);
MAKE_CFGVAR(aimbot_projectile_mode, 0); //(0 = current fov) (1 = predicted fov) (2 = distance)
MAKE_CFGVAR(aimbot_projectile_aim_method, 1); //(0 = normal) (1 = silent) (2 = psilent)
MAKE_CFGVAR(aimbot_projectile_aim_pos, 0); //(0 = auto) (1 = feet) (2 = body) (3 = head)
MAKE_CFGVAR(aimbot_projectile_fov, 30.0f);
MAKE_CFGVAR(aimbot_projectile_max_sim_targets, 1);
MAKE_CFGVAR(aimbot_projectile_max_sim_time, 2.0f);
MAKE_CFGVAR(aimbot_projectile_bbox_multipoint, true);
MAKE_CFGVAR(aimbot_projectile_splash_multipoint, 2); //(0 = disabled) (1 = enabled) (2 = preferred)
MAKE_CFGVAR(aimbot_strafe_pred, false);

MAKE_CFGVAR(fov_circle_active, false);
MAKE_CFGVAR(fov_circle_color, Color({ 255, 255, 255, 255 }));
MAKE_CFGVAR(fov_circle_alpha, 0.25f);
MAKE_CFGVAR(fov_circle_style, 1); //(0 = static) (1 = spinning spaced)
MAKE_CFGVAR(fov_circle_spacing, 2);
MAKE_CFGVAR(fov_circle_spin_speed, 0.1f);

MAKE_CFGVAR(prediction_path_enable, true);
MAKE_CFGVAR(prediction_path_duration, 2.0f);
MAKE_CFGVAR(prediction_path_color, Color({ 255,255,255,255 }));
#pragma endregion

#pragma region seed_pred

MAKE_CFGVAR(seed_pred_enable, true);
MAKE_CFGVAR(seed_pred_resync_interval, 1);
MAKE_CFGVAR(seed_pred_indicator,  true);
MAKE_CFGVAR(seed_pred_indicator_pos_x, 5);;
MAKE_CFGVAR(seed_pred_indicator_pos_y, 20);

#pragma endregion

#pragma region automation

MAKE_CFGVAR(auto_sentry_wiggler, false);
MAKE_CFGVAR(auto_sentry_wiggler_self_rodeo, false);
MAKE_CFGVAR(auto_sentry_wiggler_panic_key, static_cast<int>('H'));
MAKE_CFGVAR(auto_sentry_wiggler_distance, 0.1f);

MAKE_CFGVAR(auto_strafe_active, false);
MAKE_CFGVAR(auto_strafe_smooth, 0.5f);

MAKE_CFGVAR(auto_edge_jump_active, false);
MAKE_CFGVAR(auto_edge_jump_key, static_cast<int>(VK_MENU));

MAKE_CFGVAR(auto_detonate_active, false);
MAKE_CFGVAR(auto_detonate_mode, 0); // (0 = on key) (1 = always)
MAKE_CFGVAR(auto_detonate_key, VK_XBUTTON2);

MAKE_CFGVAR(auto_mvm_ready_up, false);
MAKE_CFGVAR(auto_disguise, false);

#pragma endregion

#pragma region lagcomp

MAKE_CFGVAR(lag_comp_visuals_active, true);
MAKE_CFGVAR(lag_comp_visuals_last_only, true);
MAKE_CFGVAR(lag_comp_visuals_mat, 0); // (0 = shaded) (1 = flat)
MAKE_CFGVAR(lag_comp_visuals_clr, Color({ 200, 200, 200, 200 }));
MAKE_CFGVAR(lag_comp_visuals_alpha, 1.0f);
MAKE_CFGVAR(lag_comp_visuals_depth, true);

#pragma endregion

#pragma region crits

MAKE_CFGVAR(crits_skip_random_crit, true);
MAKE_CFGVAR(crits_force_crit_key, VK_SHIFT);
MAKE_CFGVAR(crits_indicator, true);
MAKE_CFGVAR(crits_indicator_pos, 0.55f);
MAKE_CFGVAR(crits_experimental_fixes, true);

#pragma endregion

#pragma region tick_base

MAKE_CFGVAR(tb_recharge_key, static_cast<int>('R'));
MAKE_CFGVAR(tb_dt_key, static_cast<int>(VK_XBUTTON1));
MAKE_CFGVAR(tb_warp_key, static_cast<int>('B'));
MAKE_CFGVAR(tb_antiwarp_mode, 0);
MAKE_CFGVAR(tb_antiwarp_style, 0);
MAKE_CFGVAR(tb_dt_lock, false);
MAKE_CFGVAR(tb_indicator, true);
MAKE_CFGVAR(tb_indicator_pos, 0.7f);

#pragma endregion

#pragma region interp_abuse

MAKE_CFGVAR(interp_abuse, false);
MAKE_CFGVAR(interp_amount, 0);
MAKE_CFGVAR(interp_ratio_amount, 11);

#pragma endregion

#pragma region esp

MAKE_CFGVAR(esp_active, true);
MAKE_CFGVAR(esp_text_color, 1); // (0 = default) (1 = custom)
MAKE_CFGVAR(esp_health_color, 0) // (0 = default) (1 = custom)
MAKE_CFGVAR(esp_name_pos, 0); // (0 = top) (1 = right)
MAKE_CFGVAR(esp_faresp, true);
MAKE_CFGVAR(esp_faresp_distance, 2048);

MAKE_CFGVAR(esp_players_active, true);
MAKE_CFGVAR(esp_players_alpha, 1.0f);
MAKE_CFGVAR(esp_players_ignore_local, false);
MAKE_CFGVAR(esp_players_ignore_friends, false);
MAKE_CFGVAR(esp_players_ignore_enemies, false);
MAKE_CFGVAR(esp_players_ignore_teammates, true);
MAKE_CFGVAR(esp_players_name, true);
MAKE_CFGVAR(esp_players_class, false);
MAKE_CFGVAR(esp_players_class_icon, false);
MAKE_CFGVAR(esp_players_health, false);
MAKE_CFGVAR(esp_players_health_bar, true);
MAKE_CFGVAR(esp_players_uber, false);
MAKE_CFGVAR(esp_players_uber_bar, true);
MAKE_CFGVAR(esp_players_box, false);
MAKE_CFGVAR(esp_players_conds, true);

MAKE_CFGVAR(esp_buildings_active, true);
MAKE_CFGVAR(esp_buildings_alpha, 1.0f);
MAKE_CFGVAR(esp_buildings_ignore_local, false);
MAKE_CFGVAR(esp_buildings_ignore_enemies, false);
MAKE_CFGVAR(esp_buildings_ignore_teammates, true);
MAKE_CFGVAR(esp_buildings_name, true);
MAKE_CFGVAR(esp_buildings_health, false);
MAKE_CFGVAR(esp_buildings_health_bar, true);
MAKE_CFGVAR(esp_buildings_box, false);
MAKE_CFGVAR(esp_buildings_conds, true);

MAKE_CFGVAR(esp_projectiles_active, false);
MAKE_CFGVAR(esp_projectiles_alpha, 1.0f);
MAKE_CFGVAR(esp_projectiles_ignore_local, false);
MAKE_CFGVAR(esp_projectiles_ignore_enemies, false);
MAKE_CFGVAR(esp_projectiles_ignore_teammates, true);
MAKE_CFGVAR(esp_projectiles_name, true);
MAKE_CFGVAR(esp_projectiles_box, false);

MAKE_CFGVAR(esp_world_active, false);
MAKE_CFGVAR(esp_world_alpha, 1.0f);
MAKE_CFGVAR(esp_world_ignore_health_packs, false);
MAKE_CFGVAR(esp_world_ignore_ammo_packs, false);
MAKE_CFGVAR(esp_world_ignore_halloween_gifts, false);
MAKE_CFGVAR(esp_world_ignore_mvm_money, false);
MAKE_CFGVAR(esp_world_name, true);
MAKE_CFGVAR(esp_world_box, false);

MAKE_CFGVAR(color_esp_text, Color({ 200, 200, 200, 255 }));
MAKE_CFGVAR(color_esp_outline, Color({ 10, 10, 10, 255 }));
MAKE_CFGVAR(color_esp_low_health, Color({ 255, 110, 110, 255 }));
MAKE_CFGVAR(color_esp_high_health, Color({ 137, 255, 110, 255 }));
MAKE_CFGVAR(esp_spacing_x, 2.0f);
MAKE_CFGVAR(esp_spacing_y, 2.0f);
MAKE_CFGVAR(esp_healthbar_divisions, 1);

#pragma endregion

#pragma region outlines

MAKE_CFGVAR(outlines_active, true);
MAKE_CFGVAR(outlines_style, 1); // (0 = blur) (1 = sharp) (2 = fat)
MAKE_CFGVAR(outlines_bloom_amount, 3);
MAKE_CFGVAR(outlines_fill_occluded, false);

MAKE_CFGVAR(outlines_players_active, true);
MAKE_CFGVAR(outlines_players_alpha, 1.0f);
MAKE_CFGVAR(outlines_players_ignore_local, false);
MAKE_CFGVAR(outlines_players_ignore_friends, false);
MAKE_CFGVAR(outlines_players_ignore_enemies, false);
MAKE_CFGVAR(outlines_players_ignore_teammates, true);

MAKE_CFGVAR(outlines_buildings_active, true);
MAKE_CFGVAR(outlines_buildings_alpha, 1.0f);
MAKE_CFGVAR(outlines_buildings_ignore_local, false);
MAKE_CFGVAR(outlines_buildings_ignore_enemies, false);
MAKE_CFGVAR(outlines_buildings_ignore_teammates, true);

MAKE_CFGVAR(outlines_projectiles_active, true);
MAKE_CFGVAR(outlines_projectiles_alpha, 1.0f);
MAKE_CFGVAR(outlines_projectiles_ignore_local, false);
MAKE_CFGVAR(outlines_projectiles_ignore_enemies, false);
MAKE_CFGVAR(outlines_projectiles_ignore_teammates, true);

MAKE_CFGVAR(outlines_world_active, true);
MAKE_CFGVAR(outlines_world_alpha, 1.0f);
MAKE_CFGVAR(outlines_world_ignore_health_packs, false);
MAKE_CFGVAR(outlines_world_ignore_ammo_packs, false);
MAKE_CFGVAR(outlines_world_ignore_halloween_gifts, false);
MAKE_CFGVAR(outlines_world_ignore_mvm_money, false);

#pragma endregion

#pragma region visuals

MAKE_CFGVAR(fov_override_active, false);
MAKE_CFGVAR(fov_override_value, 120);
MAKE_CFGVAR(fov_override_value_zoomed, 120);

MAKE_CFGVAR(vm_override_active, false);
MAKE_CFGVAR(vm_override_offset_x, 0.0f);
MAKE_CFGVAR(vm_override_offset_y, 0.0f);
MAKE_CFGVAR(vm_override_offset_z, 0.0f);
MAKE_CFGVAR(vm_override_sway, 0.0f);

MAKE_CFGVAR(tp_override_active, true);
MAKE_CFGVAR(tp_override_force, false);
MAKE_CFGVAR(tp_override_force_toggle_key, static_cast<int>('H'));
MAKE_CFGVAR(tp_override_collision, true);
MAKE_CFGVAR(tp_override_lerp_time, 0.0f);
MAKE_CFGVAR(tp_override_dist_forward, 150.0f);
MAKE_CFGVAR(tp_override_dist_right, 0.0f);
MAKE_CFGVAR(tp_override_dist_up, 0.0f);

MAKE_CFGVAR(mat_mod_active, true);
MAKE_CFGVAR(mat_mod_mode, 0); // (0 = night) (1 = color)
MAKE_CFGVAR(mat_mod_night_scale, 0.5f);
MAKE_CFGVAR(mat_mod_color_world, Color({ 150, 150, 180, 255 }));
MAKE_CFGVAR(mat_mod_color_props, Color({ 150, 150, 180, 255 }));
MAKE_CFGVAR(mat_mod_color_sky, Color({ 150, 150, 180, 255 }));

MAKE_CFGVAR(particle_mod_active, false);
MAKE_CFGVAR(particle_mod_mode, 0); // (0 = color) (1 = rainbow)
MAKE_CFGVAR(particle_mod_color, Color({ 255, 255, 255, 255 }));
MAKE_CFGVAR(particle_mod_rainbow_speed, 0.5f);

MAKE_CFGVAR(remove_scope, true);
MAKE_CFGVAR(remove_punch, true);
MAKE_CFGVAR(remove_shake, true);
MAKE_CFGVAR(remove_fade, true);
MAKE_CFGVAR(remove_post_processing, true);
MAKE_CFGVAR(remove_fog, true);
MAKE_CFGVAR(remove_sky_fog, true);
MAKE_CFGVAR(remove_hands, true);
MAKE_CFGVAR(remove_interp, false);

MAKE_CFGVAR(reveal_scoreboard, true);
MAKE_CFGVAR(static_player_model_lighting, true);
MAKE_CFGVAR(spritecard_texture_hack, false);

MAKE_CFGVAR(spectator_list_active, true);
MAKE_CFGVAR(spectator_list_x, 400.0f);
MAKE_CFGVAR(spectator_list_y, 400.0f);
MAKE_CFGVAR(spectator_list_show_respawn_time, true);
MAKE_CFGVAR(spectator_list_override_firstperson, true);
MAKE_CFGVAR(spectator_list_firstperson_clr, Color(255, 255, 0, 255));

MAKE_CFGVAR(tracers_active, true);
MAKE_CFGVAR(tracers_type, 0); // 0 = beams, 1+ = particle type
MAKE_CFGVAR(tracers_mode, 0); // 0 = use color, 1 = rainbow
MAKE_CFGVAR(tracers_rainbow_speed, 0.5f);
MAKE_CFGVAR(tracers_fade, true);
MAKE_CFGVAR(tracers_color, Color({ 255, 255, 255, 255 }));

#pragma endregion

#pragma region misc

MAKE_CFGVAR(misc_bhop, true);
MAKE_CFGVAR(misc_faststop, false);
MAKE_CFGVAR(misc_sv_pure_bypass, true);
MAKE_CFGVAR(misc_noisemaker_spam, false);
MAKE_CFGVAR(misc_noisemaker_spam_toggle, static_cast<int>('N'));
MAKE_CFGVAR(misc_mvm_instant_respawn, false);
MAKE_CFGVAR(misc_rainbow_party, true);

MAKE_CFGVAR(misc_taunts_slide, true);
MAKE_CFGVAR(misc_taunts_control, true);
MAKE_CFGVAR(misc_taunts_follow_camera, false);
MAKE_CFGVAR(misc_taunts_spin, true);
MAKE_CFGVAR(misc_taunts_spin_toggle_key, static_cast<int>('R'));
MAKE_CFGVAR(misc_taunts_spin_speed, 5.0f);
MAKE_CFGVAR(misc_taunts_spin_mode, 0); // normal, random, sine, triangle

MAKE_CFGVAR(misc_remove_end_of_match_surveys, true);
MAKE_CFGVAR(misc_remove_autojoin_countdown, true);
MAKE_CFGVAR(misc_remove_equip_regions, true);
MAKE_CFGVAR(misc_remove_teammate_pushaway, false);
MAKE_CFGVAR(misc_remove_item_found_notification, true);
MAKE_CFGVAR(misc_remove_fix_angle, true);

#pragma endregion

#pragma region colors

MAKE_CFGVAR(color_local, Color({ 43, 203, 186, 255 }));
MAKE_CFGVAR(color_friend, Color({ 38, 222, 129, 255 }));
MAKE_CFGVAR(color_enemy, Color({ 250, 170, 10, 255 }));
MAKE_CFGVAR(color_teammate, Color({ 252, 92, 101, 255 }));
MAKE_CFGVAR(color_target, Color({ 224, 86, 253, 255 }));
MAKE_CFGVAR(color_invuln, Color({ 165, 94, 234, 255 }));
MAKE_CFGVAR(color_invis, Color({ 209, 216, 224, 255 }));

MAKE_CFGVAR(color_overheal, Color({ 69, 170, 242, 255 }));
MAKE_CFGVAR(color_uber, Color({ 224, 86, 253, 255 }));
MAKE_CFGVAR(color_conds, Color({ 249, 202, 36, 255 }));

MAKE_CFGVAR(color_health_pack, Color({ 46, 204, 113, 255 }));
MAKE_CFGVAR(color_ammo_pack, Color({ 200, 200, 200, 255 }));
MAKE_CFGVAR(color_halloween_gift, Color({ 255, 255, 255, 255 }));
MAKE_CFGVAR(color_mvm_money, Color({ 0, 200, 20, 255 }));

MAKE_CFGVAR(color_notifs_style_message, Color({ 180, 180, 180, 255 }));
MAKE_CFGVAR(color_notifs_style_warning, Color({ 180, 180, 0, 255 }));

#pragma endregion

#pragma region notifs

MAKE_CFGVAR(notifs_active, true);
MAKE_CFGVAR(notifs_print_to_con, true);
MAKE_CFGVAR(notifs_duration, 3.0f);
MAKE_CFGVAR(notifs_fade_in_time, 0.15f);
MAKE_CFGVAR(notifs_fade_out_time, 0.5f);

#pragma endregion

#pragma region ui	

MAKE_CFGVAR(color_ui_background, Color({ 20, 20, 20, 255 }));
MAKE_CFGVAR(color_ui_background_active, Color({ 30, 30, 30, 255 }));
MAKE_CFGVAR(color_ui_text, Color({ 180, 180, 180, 255 }));
MAKE_CFGVAR(color_ui_text_active, Color({ 230, 230, 230, 255 }));
MAKE_CFGVAR(color_ui_outline, Color({ 40, 40, 40, 255 }));

MAKE_CFGVAR(ui_scale, 0);

namespace cfg
{
	inline vec2 ui_item_size{ 150.0f, 16.0f };
	inline vec2 ui_item_pad{ 4.0f, -1.0f };
	inline vec2 ui_color_picker_preview_size{ 20.0f, 8.0f };
	inline vec2 ui_color_picker_size_b{ 150.0f, 150.0f };
	inline vec2 ui_color_picker_knob_size{ 4.0f, 4.0f };
	inline float ui_color_picker_hue_w{ 20.0f };
	inline float ui_color_picker_alpha_h{ 20.0f };
}

#pragma endregion