#pragma once

#include "helpers/draw.hpp"
#include "helpers/entity_cache.hpp"
#include "helpers/cvars.hpp"
#include "helpers/fnv1a.h"
#include "helpers/input.hpp"
#include "helpers/tf_utils.hpp"
#include "helpers/render_utils.hpp"

namespace tf_globals
{
	inline bool block_mat_override{};
	inline CUserCmd last_cmd{};
	inline bool send_packet{};
	inline bool final_tick{};
	inline std::vector<vec3> pred_path{};
	inline float pred_path_expire_time{};
	inline bool in_CBaseWorldView_DrawExecute{};
	inline EntityHandle_t aimbot_target{};
}