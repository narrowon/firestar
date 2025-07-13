#include "lag_comp.hpp"

#include "../trace_utils/trace_utils.hpp"
#include "../visual_utils/visual_utils.hpp"
#include "../cfg.hpp"

bool LagComp::isRecordValid(const LagRecord &lr)
{
	C_BasePlayer *const pl{ lr.pl_ptr };

	if (!pl || pl->deadflag()) {
		return false;
	}

	static ConVar *const sv_lagcompensation_teleport_dist{ i::cvar->FindVar("sv_lagcompensation_teleport_dist") };

	if (!sv_lagcompensation_teleport_dist) {
		return false;
	}

	const float tele_dist_sqr{ sv_lagcompensation_teleport_dist->GetFloat() * sv_lagcompensation_teleport_dist->GetFloat() };

	//normie: instead of this poopoo balls we should iterate through lag records and count up how many records there are before teleport dist check hits
	//and we'll use that as our hittable lag record size
	if (vec3{ lr.origin - pl->m_vecOrigin() }.length2DSqr() > tele_dist_sqr) {
		return false;
	}

	static ConVar *const sv_maxunlag{ i::cvar->FindVar("sv_maxunlag") };

	if (!sv_maxunlag) {
		return false;
	}

	const float cur_time{ tf_utils::getServerTime() };

	if ((cur_time - lr.sim_time) > sv_maxunlag->GetFloat()) {
		return false;
	}

	return (cur_time - lr.sim_time) < 0.2f;
}

void LagComp::addRecord(C_TFPlayer *const pl)
{
	if (!pl
		|| pl->deadflag() 
		|| pl->GetClassId() != class_ids::CTFPlayer 
		|| pl->entindex() == i::engine->GetLocalPlayer() 
		|| pl->m_flSimulationTime() <= pl->m_flOldSimulationTime()) {
		return;
	}

	std::deque<LagRecord> &recs{ m_records[pl->GetRefEHandle().GetIndex()] };

	const LagRecord lr{ pl };

	if (!isRecordValid(lr)) {
		return;
	}

	if (lr.isRecordDead())
		return;

	recs.push_front({ pl });
}

bool LagComp::onLoad()
{
	if (!m_model_mat_shaded)
	{
		KeyValues *const kv{ new KeyValues("VertexLitGeneric") }; {
			kv->SetString("$basetexture", "white");
		}

		m_model_mat_shaded = i::mat_sys->CreateMaterial("model_mat", kv);
	}

	if (!m_model_mat_flat)
	{
		KeyValues *const kv{ new KeyValues("UnlitGeneric") }; {
			kv->SetString("$basetexture", "white");
		}

		m_model_mat_flat = i::mat_sys->CreateMaterial("model_mat_flat", kv);
	}

	m_mats_loaded = true;

	return true;
}

bool LagComp::onUnload()
{
	if (m_model_mat_shaded) {
		m_model_mat_shaded->DecrementReferenceCount();
		m_model_mat_shaded->DeleteIfUnreferenced();
		m_model_mat_shaded = nullptr;
	}

	if (m_model_mat_flat) {
		m_model_mat_flat->DecrementReferenceCount();
		m_model_mat_flat->DeleteIfUnreferenced();
		m_model_mat_flat = nullptr;
	}

	m_mats_loaded = false;

	return true;
}

bool LagComp::onLevelInit()
{
	return onLoad();
}

bool LagComp::onLevelShutdown()
{
	m_records.clear();

	return onUnload();
}

void LagComp::update()
{
	for (auto it{ m_records.begin() }; it != m_records.end(); )
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(it->first) };

		if (!ent 
			|| ent->IsDormant() 
			|| !ent->GetBaseEntity() 
			|| ent->cast<C_BasePlayer>()->deadflag()
			|| ent->GetClassId() != class_ids::CTFPlayer) {
			it = m_records.erase(it);
			continue;
		}

		std::deque<LagRecord> &deque{ it->second };

		for (auto deque_it{ deque.begin() }; deque_it != deque.end(); )
		{
			if (!isRecordValid(*deque_it)) {
				deque_it = deque.erase(deque_it);
			}

			else {
				deque_it++;
			}
		}

		it++;
	}
}

size_t LagComp::getNumRecords(C_TFPlayer *const pl)
{
	if (!pl) {
		return 0;
	}

	const EntityHandle_t ehandle{ pl->GetRefEHandle() };

	if (!m_records.contains(ehandle.GetIndex())) {
		return 0;
	}

	return m_records[ehandle.GetIndex()].size();
}

const LagRecord *const LagComp::getRecord(C_TFPlayer *const pl, const size_t record_idx)
{
	if (!pl) {
		return nullptr;
	}

	const EntityHandle_t ehandle{ pl->GetRefEHandle() };

	if (!m_records.contains(ehandle.GetIndex()) || record_idx >= m_records[ehandle.GetIndex()].size()) {
		return nullptr;
	}

	return &m_records[ehandle.GetIndex()][record_idx];
}

void LagComp::drawLagCompModel(C_TFPlayer *const pl, const LagRecord *const lr)
{
	if (!pl || !lr) {
		return;
	}

	m_drawing_models = true;
	lrm->set(pl, lr);
	pl->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
	lrm->reset();
	m_drawing_models = false;
}

void LagComp::visual()
{
	if (!cfg::lag_comp_visuals_active || !m_mats_loaded) {
		return;
	}

	C_TFPlayer *const local{ ec->getLocal() };

	if (!local || local->deadflag()) {
		return;
	}

	C_TFWeaponBase *const weapon{ ec->getWeapon() };

	if (!weapon || (!tf_utils::isWeaponHitscan(weapon) && !tf_utils::isWeaponMelee(weapon))) {
		return;
	}

	IMatRenderContext *const rc{ i::mat_sys->GetRenderContext() };

	if (!rc) {
		return;
	}

	i::model_render->ForcedMaterialOverride(cfg::lag_comp_visuals_mat == 0 ? m_model_mat_shaded : m_model_mat_flat);

	i::render_view->SetColorModulation(cfg::lag_comp_visuals_clr);
	i::render_view->SetBlend(cfg::lag_comp_visuals_alpha);

	if (!cfg::lag_comp_visuals_depth) {
		rc->DepthRange(0.0f, 0.2f);
	}

	const bool weapon_supports_friendly_fire{ tf_utils::attribHookValue(0.0f, "speed_buff_ally", weapon) > 0.0f };
	const bool is_using_medigun{ weapon->GetWeaponID() == TF_WEAPON_MEDIGUN };

	for (const CBaseHandle ehandle : ec->getGroup(ECGroup::PLAYERS_ALL))
	{
		IClientEntity *const ent{ i::ent_list->GetClientEntityFromHandle(ehandle) };

		if (!ent) {
			continue;
		}

		C_TFPlayer *const pl{ ent->cast<C_TFPlayer>() };

		if (!pl || pl->deadflag() || getNumRecords(pl) <= 0) {
			continue;
		}

        const auto local_team{ local->m_iTeamNum() };
        const auto pl_team{ pl->m_iTeamNum() };
		
		if (!(weapon_supports_friendly_fire || is_using_medigun) && pl_team == local_team) {
            continue;
        }

		if (is_using_medigun && pl_team != local_team) {
			continue;
		}

		for (size_t n{ getNumRecords(pl) }; n-- > 0; )
		{
			const LagRecord *const lr{ getRecord(pl, n) };

			if (!lr || !vis_utils->isPosOnScreen(local, lr->render_origin) || lr->hitboxes.empty()) {
				continue;
			}

			vec3 head{};

			if (!tf_utils::getHitbox(pl, HITBOX_HEAD, head) || head.distTo(lr->hitboxes[0]) < 2.0f) {
				continue;
			}

			drawLagCompModel(pl, lr);

			if (cfg::lag_comp_visuals_last_only) {
				break;
			}
		}
	}

	i::model_render->ForcedMaterialOverride(nullptr);

	i::render_view->SetColorModulation({ 255, 255, 255, 255 });
	i::render_view->SetBlend(1.0f);

	if (!cfg::lag_comp_visuals_depth) {
		rc->DepthRange(0.0f, 1.0f);
	}
}

bool LagComp::isDrawingModels() const
{
	return m_drawing_models;
}

LagRecord::LagRecord(C_TFPlayer* const pl)
{
	if (!pl || pl->GetClassId() != class_ids::CTFPlayer) {
		return;
	}

	pl_ptr = pl;
	sim_time = base_sim_time = pl->m_flSimulationTime();
	tick_count = i::global_vars->tickcount;
	flags = pl->m_fFlags();
	origin = pl->m_vecOrigin();
	center = pl->GetCenter();
	vel = pl->m_vecVelocity();
	render_origin = pl->GetRenderOrigin();
	abs_origin = pl->GetAbsOrigin();
	abs_angles = pl->GetAbsAngles();
	mins = pl->m_vecMinsPreScaled();
	maxs = pl->m_vecMaxsPreScaled();
	eye_angs = pl->m_angEyeAngles();

	bones_setup = false;


	if (!bones) {
		bones = static_cast<matrix3x4_t*>(i::mem_alloc->Alloc(sizeof(matrix3x4_t) * 128));
	}

	if (bones)
	{
		lrm->allowBoneSetup(true);

		pl->InvalidateBoneCache();
		pl->m_fEffects() |= 8; //ef_nointerp

		bones_setup = pl->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, pl->m_flSimulationTime());

		pl->m_fEffects() &= ~8;

		lrm->allowBoneSetup(false);

		for (int n{}; n < tf_utils::getHitboxCount(pl); n++)
		{
			vec3 hbox{};

			if (!tf_utils::getHitbox(pl, bones, n, hbox)) {
				continue;
			}

			hitboxes.push_back(hbox);
		}
	}
}

LagRecord::~LagRecord()
{
	/*if (bones) {
		i::mem_alloc->Free(bones);
		bones = nullptr;
	}*/
}

float GetInterpolationTime()
{
	static ConVar* sv_minupdaterate = i::cvar->FindVar("sv_minupdaterate");
	static ConVar* sv_maxupdaterate = i::cvar->FindVar("sv_maxupdaterate");
	static ConVar* cl_updaterate = i::cvar->FindVar("cl_updaterate");
	static ConVar* cl_interpolate = i::cvar->FindVar("cl_interpolate");
	static ConVar* cl_interp_ratio = i::cvar->FindVar("cl_interp_ratio");
	static ConVar* cl_interp = i::cvar->FindVar("cl_interp");
	static ConVar* sv_client_min_interp_ratio = i::cvar->FindVar("sv_client_min_interp_ratio");
	static ConVar* sv_client_max_interp_ratio = i::cvar->FindVar("sv_client_max_interp_ratio");

	float lerp_time = 0.f;

	if (cl_interpolate->GetInt() != 0)
	{
		int update_rate = std::clamp(cl_updaterate->GetInt(), (int)sv_minupdaterate->GetFloat(), (int)sv_maxupdaterate->GetFloat());

		float lerp_ratio = cl_interp_ratio->GetFloat();
		if (lerp_ratio == 0)
			lerp_ratio = 1.f;

		float lerp_amount = cl_interp->GetFloat();

		if (sv_client_min_interp_ratio && sv_client_max_interp_ratio && sv_client_min_interp_ratio->GetFloat() != -1.f)
		{
			lerp_ratio = std::clamp(lerp_ratio, sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());
		}
		else
		{
			if (lerp_ratio == 0.f)
				lerp_ratio = 1.f;
		}

		lerp_time = std::max(lerp_amount, lerp_ratio / update_rate);
	}

	return lerp_time;
}


const bool LagRecord::isRecordValid(float add_time) const
{
	if (!hasSetupBones()) {
		return false;
	}

	auto* net_channel = i::engine->GetNetChannelInfo();

	if (!net_channel) {
		return false;
	}

	// Estimate server arrival tick
	int estimated_arrival_tick = i::global_vars->tickcount + 1;
	estimated_arrival_tick += tf_utils::timeToTicks(net_channel->GetLatency(FLOW_OUTGOING) + net_channel->GetLatency(FLOW_INCOMING));

	// Compute "correct" time
	float correct = GetInterpolationTime();
	correct += net_channel->GetLatency(FLOW_OUTGOING) + net_channel->GetLatency(FLOW_INCOMING);
	correct = std::clamp(correct, 0.0f, cvars::sv_maxunlag->GetFloat());

	// Calculate target tick from base_sim_time
	int target_tick = tf_utils::timeToTicks(base_sim_time);
	float delta_time = correct - tf_utils::ticksToTime(estimated_arrival_tick - target_tick);

	// Absolute difference between predicted arrival and record's simulation time
	float diff = tf_utils::ticksToTime(estimated_arrival_tick - tf_utils::timeToTicks(base_sim_time));

	// Validate timing window
	if (std::fabs(delta_time) > 0.2f || correct >= 1.0f || diff >= 1.0f) {
		return false;
	}

	return true;
}

const bool LagRecord::isRecordDead() const
{
	if (C_TFPlayer* const pl{ this->pl_ptr }; !pl || pl->deadflag()) {
		return true;
	}

	const float cur_time{ tf_utils::getServerTime() + tf_utils::getLatency() };

	auto* net_channel = i::engine->GetNetChannelInfo();
	if (!net_channel) {
		return true;
	}

	const float total_latency =
		net_channel->GetLatency(FLOW_OUTGOING) +
		net_channel->GetLatency(FLOW_INCOMING);

	const float max_unlag = std::clamp(cvars::sv_maxunlag->GetFloat(), 0.0f, 1.0f);
	const float dead_time = cur_time - max_unlag;

	if (this->base_sim_time < dead_time || this->base_sim_time > cur_time + 0.2f) {
		return true;
	}

	if (this->tick_count > 0) {
		const int current_tick = i::global_vars->tickcount;
		const int max_tick_diff = static_cast<int>(max_unlag / i::global_vars->interval_per_tick);

		if (current_tick - this->tick_count > max_tick_diff) {
			return true;
		}
	}

	//if (!cfg::fake_ping_enable) { 
	if (!canRestoreToSimulationTime(this->base_sim_time))
		return true; // mark as dead if can't restore
	float delta = tf_utils::ticksToTime(i::global_vars->tickcount) - this->arrive_time;
	if (delta < limit(this->sim_time)) // keep record alive longer for fake ping
		return false;
	//}

	if (isDeltaTooBig(this->abs_origin, this->pl_ptr->GetAbsOrigin())) // if the record being checked - the players current abs_origin squared o 4096.f, we set it as fucking tru
		return true;

	return false;
}

const bool LagRecord::hasSetupBones() const
{
	return bones != nullptr && bones_setup;
}

const bool LagRecord::isDeltaTooBig(const vec3& from, const vec3& to) const
{
	return (from - to).lengthSqr() > 4096.f;
}

const bool LagRecord::canRestoreToSimulationTime(float sim_time) const
{
	float correct = GetInterpolationTime();

	auto* net_channel = i::engine->GetNetChannelInfo();

	if (net_channel)
		correct += net_channel->GetLatency(FLOW_OUTGOING) + net_channel->GetLatency(FLOW_INCOMING);

	correct = std::clamp(correct, 0.0f, 1.0f);

	int target_tick = tf_utils::timeToTicks(sim_time);
	float delta_time = correct - tf_utils::ticksToTime(estServerArriveTick() - target_tick);

	float diff = tf_utils::ticksToTime(estServerArriveTick() - tf_utils::timeToTicks(sim_time));

	if (fabsf(delta_time) > 0.2f || correct >= 1.f || diff >= 1.f)
	{
		return false;
	}

	return true;
}

const int LagRecord::estServerArriveTick() const
{
	int tick = i::global_vars->tickcount + 1;

	auto* net_channel = i::engine->GetNetChannelInfo();
	if (net_channel)
		tick += tf_utils::timeToTicks(net_channel->GetLatency(FLOW_OUTGOING) + net_channel->GetLatency(FLOW_INCOMING));

	return tick;
}

const float LagRecord::limit(float sim_time) const
{
	float correct = GetInterpolationTime();

	auto* net_channel = i::engine->GetNetChannelInfo();

	if (net_channel)
		correct += net_channel->GetLatency(FLOW_OUTGOING) + net_channel->GetLatency(FLOW_INCOMING);

	correct = std::clamp(correct, 0.0f, 1.0f);

	int target_tick = tf_utils::timeToTicks(sim_time);
	float time = correct - tf_utils::ticksToTime(estServerArriveTick() - target_tick);

	return time;
}
