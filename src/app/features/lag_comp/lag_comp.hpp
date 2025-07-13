#pragma once

#include "../../../game/game.hpp"

class LagRecord final
{
public:
	C_TFPlayer* pl_ptr{};
	float sim_time{};
	float base_sim_time{};
	float arrive_time{};
	int tick_count{};
	int flags{};
	vec3 origin{};
	vec3 center{};
	vec3 vel{};
	vec3 render_origin{};
	vec3 abs_origin{};
	vec3 abs_angles{};

	vec3 mins{};
	vec3 maxs{};
	vec3 eye_angs{};
	std::vector<LagRecord>* pRecords;  // Pointer to a vector of lag records

	matrix3x4_t* bones = nullptr;
	bool bones_setup{};
	std::vector<vec3> hitboxes{};
	bool interpolated{};

public:
	LagRecord(C_TFPlayer* const pl);
	~LagRecord();

public:
	const bool isRecordValid(float add_time = 0.f) const;
	const bool isRecordDead() const;
	const bool hasSetupBones() const;
	const bool isDeltaTooBig(const vec3& from, const vec3& to) const;
	const bool canRestoreToSimulationTime(float sim_time) const;
	const int estServerArriveTick() const;
	const float limit(float sim_time) const;
};

//s: why does this work while AimUtils::SetLagRecord ctor/dtor method I had earlier didnt(for tracing).. FUCK
class LagRecordMatrixHelper
{
private:
	C_TFPlayer *m_pl{};
	vec3 m_abs_origin{};
	vec3 m_abs_angles{};
	vec3 m_mins{};
	vec3 m_maxs{};
	std::array<matrix3x4_t, 128> m_bones{};
	bool m_allow_bone_setup{};
public:
	void set(C_TFPlayer *const pl, const LagRecord *const lr)
	{
		if (!pl || !lr) {
			return;
		}

		CUtlVector<matrix3x4_t> bones{ pl->m_CachedBoneData() };

		m_pl = pl;
		m_abs_origin = pl->GetAbsOrigin();
		m_abs_angles = pl->GetAbsAngles();

		m_mins = pl->m_vecMinsPreScaled();
		m_maxs = pl->m_vecMaxsPreScaled();

		memcpy(m_bones.data(), bones.Base(), sizeof(matrix3x4_t) * bones.Count());
		memcpy(bones.Base(), lr->bones, sizeof(matrix3x4_t) * bones.Count());

		pl->SetAbsOrigin(lr->abs_origin);
		pl->SetAbsAngles(lr->abs_angles);
		pl->SetCollisionBounds(lr->mins, lr->maxs);
	}

	void reset()
	{
		if (!m_pl) {
			return;
		}

		CUtlVector<matrix3x4_t> bones{ m_pl->m_CachedBoneData() };

		m_pl->SetAbsOrigin(m_abs_origin);
		m_pl->SetAbsAngles(m_abs_angles);
		m_pl->SetCollisionBounds(m_mins, m_maxs);

		memcpy(bones.Base(), m_bones.data(), sizeof(matrix3x4_t) * bones.Count());

		m_pl = nullptr;
		m_abs_origin = {};
		m_abs_angles = {};
		m_mins = {};
		m_maxs = {};

		m_bones = {};
	}

	inline bool isBoneSetupAllowed() const {
		return m_allow_bone_setup;
	}

	inline void allowBoneSetup(bool state) {
		m_allow_bone_setup = state;
	}
};

class LagComp final : public HasLoad, public HasUnload, public HasLevelInit, public HasLevelShutdown
{
private:
	std::unordered_map<unsigned long, std::deque<LagRecord>> m_records{};

private:
	IMaterial *m_model_mat_shaded{};
	IMaterial *m_model_mat_flat{};

private:
	bool m_mats_loaded{};
	bool m_drawing_models{};

private:
	bool isRecordValid(const LagRecord &lr);

public:
	void addRecord(C_TFPlayer*const pl);

public:
	bool onLoad() override;
	bool onUnload() override;
	bool onLevelInit() override;
	bool onLevelShutdown() override;

public:
	void update();
	size_t getNumRecords(C_TFPlayer *const pl);
	const LagRecord *const getRecord(C_TFPlayer *const pl, const size_t record_idx);

private:
	void drawLagCompModel(C_TFPlayer *const pl, const LagRecord *const lr);

public:
	void visual();
	bool isDrawingModels() const;
};

MAKE_UNIQUE(LagComp, lag_comp);
MAKE_UNIQUE(LagRecordMatrixHelper, lrm);