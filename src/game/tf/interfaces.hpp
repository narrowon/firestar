#pragma once

#include "entities.hpp"

class IGameEventListener2;

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses()
	{
		return mem::callVirtual<8, ClientClass*>(this);
	}

	void CreateMove(int sequence_number, float input_sample_frametime, bool active)
	{
		mem::callVirtual<21, void, int, float, bool>(this, sequence_number, input_sample_frametime, active);
	}

	bool WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand)
	{
		return mem::callVirtual<23, bool, bf_write*, int, int, bool>(this, buf, from, to, isnewcommand);
	}

	CStandardRecvProxies* GetStandardRecvProxies()
	{
		return mem::callVirtual<46, CStandardRecvProxies*>(this);
	}

	int GetScreenWidth()
	{
		return mem::callVirtual<56, int>(this);
	}

	int GetScreenHeight()
	{
		return mem::callVirtual<57, int>(this);
	}

	bool GetPlayerView(CViewSetup& playerView)
	{
		return mem::callVirtual<59, bool, CViewSetup&>(this, playerView);
	}
};
struct color_rgb_exp_32
{
	byte r, g, b;
	signed char exponent;
};

struct dlight_t
{
	int				 flags;
	vec3			 origin;
	float			 radius;
	color_rgb_exp_32 color;
	float			 die;
	float			 decay;
	float			 minlight;
	int				 key;
	int				 style;

	vec3	m_Direction;
	float	m_InnerAngle;
	float	m_OuterAngle;

	float get_radius() const {
		return radius;
	}
	float get_radius_squared() const {
		return radius * radius;
	}
	float is_radius_greater_than_zero() const {
		return radius > 0.0f;
	}
};
#define	MAX_DLIGHTS		32

class i_material;
class c_engine_effects
{
public:
	// Retrieve decal texture index from decal by name
	virtual	int				Draw_DecalIndexFromName(char* name) = 0;

	// Apply decal
	virtual	void			DecalShoot(int textureIndex, int entity,
		const model_t* model, const vec3& model_origin, const vec3& model_angles,
		const vec3& position, const vec3* saxis, int flags) = 0;

	// Apply colored decal
	virtual	void			DecalColorShoot(int textureIndex, int entity,
		const model_t* model, const vec3& model_origin, const vec3& model_angles,
		const vec3& position, const vec3* saxis, int flags, const Color& rgbaColor) = 0;

	virtual void			PlayerDecalShoot(i_material* material, void* userdata, int entity, const model_t* model,
		const vec3& model_origin, const vec3& model_angles,
		const vec3& position, const vec3* saxis, int flags, const Color& rgbaColor) = 0;

	// Allocate a dynamic world light ( key is the entity to whom it is associated )
	virtual	dlight_t* CL_AllocDlight(int key) = 0;

	// Allocate a dynamic entity light ( key is the entity to whom it is associated )
	virtual	dlight_t* CL_AllocElight(int key) = 0;

	// Get a list of the currently-active dynamic lights.
	virtual int CL_GetActiveDLights(dlight_t* pList[MAX_DLIGHTS]) = 0;

	// Retrieve decal texture name from decal by index
	virtual	const char* Draw_DecalNameFromIndex(int nIndex) = 0;

	// Given an elight key, find it. Does not search ordinary dlights. May return NULL.
	virtual dlight_t* GetElightByKey(int key) = 0;
};
class IClientEntityList
{
public:
	IClientEntity* GetClientEntity(int entnum)
	{
		return mem::callVirtual<3, IClientEntity*, int>(this, entnum);
	}

	IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt)
	{
		return mem::callVirtual<4, IClientEntity*, CBaseHandle>(this, hEnt);
	}

	int GetHighestEntityIndex()
	{
		return mem::callVirtual<6, int>(this);
	}
};

class ICvar
{
public:
	ConVar* FindVar(const char* var_name)
	{
		return mem::callVirtual<12, ConVar*, const char*>(this, var_name);
	}

	void ConsoleColorPrintf(const Color& clr, const char* pFormat, ...)
	{
		mem::callVirtual<23, ConVar*, const Color&, const char*>(this, clr, pFormat);
	}
};

class IVEngineServer
{
public:
	INetChannelInfo* GetPlayerNetInfo(int playerIndex)
	{
		return mem::callVirtual<19, INetChannelInfo*, int>(this, playerIndex);
	}
};

class IVEngineClient
{
public:
	bool GetPlayerInfo(int ent_num, player_info_t* pinfo)
	{
		return mem::callVirtual<8, bool, int, player_info_t*>(this, ent_num, pinfo);
	}

	int GetPlayerForUserID(int user_id)
	{
		return mem::callVirtual<9, int, int>(this, user_id);
	}

	int GetLocalPlayer()
	{
		return mem::callVirtual<12, int>(this);
	}

	float Time()
	{
		return mem::callVirtual<14, float>(this);
	}

	void GetViewAngles(QAngle& va)
	{
		mem::callVirtual<19, void, QAngle&>(this, va);
	}

	Vector GetViewAngles()
	{
		Vector out{};

		GetViewAngles(out);

		return out;
	}

	void SetViewAngles(QAngle& va)
	{
		mem::callVirtual<20, void, QAngle&>(this, va);
	}

	int GetMaxClients()
	{
		return mem::callVirtual<21, int>(this);
	}

	const char* Key_LookupBinding(const char* pBinding)
	{
		return mem::callVirtual<22, const char*, const char*>(this, pBinding);
	}

	float GetLastTimestamp()
	{
		return mem::callVirtual<15, float>(this);
	}

	bool IsInGame()
	{
		return mem::callVirtual<26, bool>(this);
	}

	bool IsConnected()
	{
		return mem::callVirtual<27, bool>(this);
	}

	const char* GetGameDirectory()
	{
		return mem::callVirtual<35, const char*>(this);
	}

	INetChannelInfo* GetNetChannelInfo()
	{
		return mem::callVirtual<72, INetChannelInfo*>(this);
	}

	bool IsPaused()
	{
		return mem::callVirtual<84, INetChannelInfo*>(this);
	}

	bool IsTakingScreenshot()
	{
		return mem::callVirtual<85, bool>(this);
	}

	void ServerCmdKeyValues(KeyValues* kv)
	{
		mem::callVirtual<127, void, KeyValues*>(this, kv);
	}

	void FireEvents()
	{
		mem::callVirtual<56, void>(this);
	}

	void ClientCmd_Unrestricted(const char* szCmdString)
	{
		mem::callVirtual<106, void, const char*>(this, szCmdString);
	}

	bool IsActiveApp()
	{
		return mem::callVirtual<141, bool>(this);
	}
};

#define VOL_NORM		1.0f
#define	PITCH_NORM		100	
#define PITCH_LOW		95
#define PITCH_HIGH		120

enum soundlevel_t;

class IRecipientFilter {
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};

struct SndInfo_t {
	int			m_nGuid;
	void* m_filenameHandle;
	int			m_nSoundSource;
	int			m_nChannel;
	int			m_nSpeakerEntity;
	float		m_flVolume;
	float		m_flLastSpatializedVolume;
	float		m_flRadius;
	int			m_nPitch;
	Vector* m_pOrigin;
	Vector* m_pDirection;
	bool		m_bUpdatePositions;
	bool		m_bIsSentence;
	bool		m_bDryMix;
	bool		m_bSpeaker;
	bool		m_bSpecialDSP;
	bool		m_bFromServer;
};

class IEngineSound {
public:
	virtual bool PrecacheSound(const char* pSample, bool bPreload = false, bool bIsUISound = false) = 0;
	virtual bool IsSoundPrecached(const char* pSample) = 0;
	virtual void PrefetchSound(const char* pSample) = 0;
	virtual float GetSoundDuration(const char* pSample) = 0;
	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSample, float flVolume, float flAttenuation, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0, const Vector* pOrigin = NULL, const Vector* pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;
	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSample, float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0, const Vector* pOrigin = NULL, const Vector* pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;
	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex, float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0, const Vector* pOrigin = NULL, const Vector* pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;
	virtual void StopSound(int iEntIndex, int iChannel, const char* pSample) = 0;
	virtual void StopAllSounds(bool bClearBuffers) = 0;
	virtual void SetRoomType(IRecipientFilter& filter, int roomType) = 0;
	virtual void SetPlayerDSP(IRecipientFilter& filter, int dspType, bool fastReset) = 0;
	virtual void EmitAmbientSound(const char* pSample, float flVolume, int iPitch = PITCH_NORM, int flags = 0, float soundtime = 0.0f) = 0;
	virtual float GetDistGainFromSoundLevel(soundlevel_t soundlevel, float dist) = 0;
	virtual int	GetGuidForLastSoundEmitted() = 0;
	virtual bool IsSoundStillPlaying(int guid) = 0;
	virtual void StopSoundByGuid(int guid) = 0;
	virtual void SetVolumeByGuid(int guid, float fvol) = 0;
	void GetActiveSounds(CUtlVector< SndInfo_t >& sndlist)
	{
		return mem::callVirtual<17, void, CUtlVector<SndInfo_t>&>(this, sndlist);
	}
	virtual void PrecacheSentenceGroup(const char* pGroupName) = 0;
	virtual void NotifyBeginMoviePlayback() = 0;
	virtual void NotifyEndMoviePlayback() = 0;
}; // "IEngineSoundClient"

class IEngineTrace
{
public:
	int GetPointContents(const Vector& vecAbsPosition, IHandleEntity** ppEntity)
	{
		return mem::callVirtual<0, int, const Vector&, IHandleEntity**>(this, vecAbsPosition, ppEntity);
	}

	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)
	{
		mem::callVirtual<4, void, const Ray_t&, unsigned int, ITraceFilter*, trace_t*>(this, ray, fMask, pTraceFilter, pTrace);
	}

	void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEnt, trace_t* pTrace)
	{
		mem::callVirtual<2, void, const Ray_t&, unsigned int, IHandleEntity*, trace_t*>(this, ray, fMask, pEnt, pTrace);
	}
};

class IEngineVGui
{
public:
	bool IsGameUIVisible()
	{
		return mem::callVirtual<2, bool>(this);
	}
};

struct StartSoundParams_t
{
	bool			staticsound;
	int				userdata;
	int				soundsource;
	int				entchannel;
	DWORD* pSfx;
	vec3			origin;
	vec3			direction;
	bool			bUpdatePositions;
	float			fvol;
	soundlevel_t	soundlevel;
	int				flags;
	int				pitch;
	int				specialdsp;
	bool			fromserver;
	float			delay;
	int				speakerentity;
	bool			suppressrecording;
	int				initialStreamPosition;
};

class CIncomingSequence
{
public:
	int m_nInReliableState;
	int m_nSequenceNr;
	float m_flTime;
	int curtime = 0;

	CIncomingSequence(int iState, int iSequence, float flTime)
	{
		m_nInReliableState = iState;
		m_nSequenceNr = iSequence;
		m_flTime = flTime;
	}
};

class IGameMovement
{
public:
	void ProcessMovement(C_BasePlayer* pPlayer, CMoveData* pMove)
	{
		mem::callVirtual<1, void, C_BasePlayer*, CMoveData*>(this, pPlayer, pMove);
	}

	void StartTrackPredictionErrors(C_BasePlayer* pPlayer)
	{
		mem::callVirtual<2, void, C_BasePlayer*>(this, pPlayer);
	}

	void FinishTrackPredictionErrors(C_BasePlayer* pPlayer)
	{
		mem::callVirtual<3, void, C_BasePlayer*>(this, pPlayer);
	}
};


class IUniformRandomStream
{
public:
	virtual void SetSeed(int iSeed) = 0;
	virtual float RandomFloat(float flMinVal = 0.f, float flMaxVal = 1.f) = 0;
	virtual int	RandomInt(int iMinVal, int iMaxVal) = 0;
	virtual float RandomFloatExp(float flMinVal = 0.f, float flMaxVal = 1.f, float flExponent = 1.f) = 0;
};

class CGlobalVarsBase
{
public:
	float realtime{};
	int framecount{};
	float absoluteframetime{};
	float curtime{};
	float frametime{};
	int maxClients{};
	int tickcount{};
	float interval_per_tick{};
	float interpolation_amount{};
	int simTicksThisFrame{};
	int network_protocol{};
	CSaveRestoreData* pSaveData{};
	bool m_bClient{};
	int nTimestampNetworkingBase{};
	int nTimestampRandomizeWindow{};
};

class IInput
{
public:
	char pad_0000[0x108];

public:
	CUserCmd* m_pCommands{};
	CVerifiedUserCmd* m_pVerifiedCommands{};

public:
	void CAM_ToThirdPerson()
	{
		mem::callVirtual<32, void>(this);
	}

	void CAM_ToFirstPerson()
	{
		mem::callVirtual<33, void>(this);
	}
};

class IInputSystem
{
public:
	void ResetInputState()
	{
		mem::callVirtual<25, void>(this);
	}


};

// << 2
class IMaterialSystem
{
public:
	IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues)
	{
		return mem::callVirtual<70, IMaterial*, const char*, KeyValues*>(this, pMaterialName, pVMTKeyValues);
	}

	IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = nullptr)
	{
		return mem::callVirtual<71, IMaterial*, char const*, const char*, bool, const char*>(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	MaterialHandle_t FirstMaterial()
	{
		return mem::callVirtual<73, MaterialHandle_t>(this);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t h)
	{
		return mem::callVirtual<74, MaterialHandle_t, MaterialHandle_t>(this, h);
	}

	MaterialHandle_t InvalidMaterial()
	{
		return mem::callVirtual<75, MaterialHandle_t>(this);
	}

	IMaterial* GetMaterial(MaterialHandle_t h)
	{
		return mem::callVirtual<76, IMaterial*, MaterialHandle_t>(this, h);
	}

	ITexture* FindTexture(char const* pTextureName, const char* pTextureGroupName, bool complain = true, int nAdditionalCreationFlags = 0)
	{
		return mem::callVirtual<79, ITexture*, char const*, const char*, bool, int>(this, pTextureName, pTextureGroupName, complain, nAdditionalCreationFlags);
	}

	ITexture* CreateNamedRenderTargetTextureEx(const char* pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth, unsigned int textureFlags, unsigned int renderTargetFlags)
	{
		return mem::callVirtual<85, ITexture*, const char*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int>(this, pRTName, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags);
	}

	IMatRenderContext* GetRenderContext()
	{
		return mem::callVirtual<98, IMatRenderContext*>(this);
	}

	ITexture* CreateTextureFromBits(int w, int h, int mips, ImageFormat fmt, int srcBufferSize, byte* srcBits)
	{
		return mem::callVirtual<125, ITexture*, int, int, int, ImageFormat, int, byte*>(this, w, h, mips, fmt, srcBufferSize, srcBits);
	}
};

class IVModelInfo
{
public:
	int GetModelIndex(const char* name)
	{
		return mem::callVirtual<2, int, const char*>(this, name);
	}
	int PrecacheModel(const char* name)
	{
		return this->GetModelIndex(name);
	}
	const char* GetModelName(const model_t* model)
	{
		return mem::callVirtual<3, const char*, const model_t*>(this, model);
	}

	studiohdr_t* GetStudioModel(const model_t* mod)
	{
		return mem::callVirtual<28, studiohdr_t*, const model_t*>(this, mod);
	}
};

class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL)
	{
		mem::callVirtual<1, void, IMaterial*, OverrideType_t>(this, newMaterial, nOverrideType);
	}

	void GetMaterialOverride(IMaterial** ppOutForcedMaterial, OverrideType_t* pOutOverrideType)
	{
		mem::callVirtual<25, void, IMaterial**, OverrideType_t*>(this, ppOutForcedMaterial, pOutOverrideType);
	}
};

class IPhysicsSurfaceProps;

class IMoveHelper
{
public:
	virtual	char const* GetName(EntityHandle_t handle) const = 0;
	virtual void	ResetTouchList(void) = 0;
	virtual bool	AddToTouched(const CGameTrace& tr, const Vector& impactvelocity) = 0;
	virtual void	ProcessImpacts(void) = 0;
	virtual void	Con_NPrintf(int idx, char const* fmt, ...) = 0;
	virtual void	StartSound(const Vector& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch) = 0;
	virtual void	StartSound(const Vector& origin, const char* soundname) = 0;
	virtual void	PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector& origin, Vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;
	virtual bool	PlayerFallingDamage(void) = 0;
	virtual void	PlayerSetAnimation(DWORD playerAnim) = 0;
	virtual IPhysicsSurfaceProps* GetSurfaceProps(void) = 0;
	virtual bool IsWorldEntity(const CBaseHandle& handle) = 0;
	virtual void	SetHost(C_BasePlayer* host) = 0;

	C_BasePlayer* GetHost()
	{
		return *reinterpret_cast<C_BasePlayer**>(reinterpret_cast<uintptr_t>(this) + 40);
	}

protected:
	virtual ~IMoveHelper() {}
};

class IPrediction
{
public:
	virtual ~IPrediction(void) {};
	virtual void	Init(void) = 0;
	virtual void	Shutdown(void) = 0;
	virtual void	Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void	PostEntityPacketReceived(void) = 0;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) = 0;
	virtual void	OnReceivedUncompressedPacket(void) = 0;
	virtual void	GetViewOrigin(Vector& org) = 0;
	virtual void	SetViewOrigin(Vector& org) = 0;
	virtual void	GetViewAngles(QAngle& ang) = 0;
	virtual void	SetViewAngles(QAngle& ang) = 0;
	virtual void	GetLocalViewAngles(QAngle& ang) = 0;
	virtual void	SetLocalViewAngles(QAngle& ang) = 0;
};

class CPrediction : public IPrediction
{
public:
	virtual			~CPrediction() {};
	virtual void	Init() = 0;
	virtual void	Shutdown() = 0;
	virtual void	Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;
	virtual void	OnReceivedUncompressedPacket() = 0;
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void	PostEntityPacketReceived() = 0;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) = 0;
	virtual bool	InPrediction() = 0;
	virtual bool	IsFirstTimePredicted() = 0;
	virtual int		GetIncomingPacketNumber() = 0;
	virtual void	GetViewOrigin(Vector& org) = 0;
	virtual void	SetViewOrigin(Vector& org) = 0;
	virtual void	GetViewAngles(QAngle& ang) = 0;
	virtual void	SetViewAngles(QAngle& ang) = 0;
	virtual void	GetLocalViewAngles(QAngle& ang) = 0;
	virtual void	SetLocalViewAngles(QAngle& ang) = 0;
	virtual void	RunCommand(C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper) = 0;
	virtual void	SetupMove(C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move) = 0;
	virtual void	FinishMove(C_BasePlayer* player, CUserCmd* ucmd, CMoveData* move) = 0;
	virtual void	SetIdealPitch(C_BasePlayer* player, const Vector& origin, const QAngle& angles, const Vector& viewheight) = 0;
	virtual void	_Update
	(
		bool received_new_world_update,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;

	CHandle< C_BaseEntity > m_hLastGround;
	bool			m_bInPrediction;
	bool			m_bFirstTimePredicted;
	bool			m_bOldCLPredictValue;
	bool			m_bEnginePaused;
	int				m_nPreviousStartFrame;
	int				m_nCommandsPredicted;
	int				m_nServerCommandsAcknowledged;
	int				m_bPreviousAckHadErrors;
	int				m_bPreviousAckErrorTriggersFullLatchReset;
	CUtlVector< CHandle< C_BaseEntity > > m_EntsWithPredictionErrorsInLastAck;
	int				m_nIncomingPacketNumber;
	float			m_flIdealPitch;
};

//class IPrediction
//{
//public:
//	void *thisptr{};
//
//public:
//	void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command)
//	{
//		mem::callVirtual<3, void, int, bool, int, int>(this, startframe, validframe, incoming_acknowledged, outgoing_command);
//	}
//
//	void SetLocalViewAngles(QAngle &ang)
//	{
//		mem::callVirtual<13, void, QAngle &>(this, ang);
//	}
//
//	void RunCommand(C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
//	{
//		mem::callVirtual<17, void, C_BasePlayer*, CUserCmd*, IMoveHelper*>(this, player, ucmd, moveHelper);
//	}
//
//	void SetupMove(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move)
//	{
//		mem::callVirtual<18, void, C_BasePlayer *, CUserCmd *, IMoveHelper *, CMoveData *>(this, player, ucmd, pHelper, move);
//	}
//
//	void FinishMove(C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move)
//	{
//		mem::callVirtual<19, void, C_BasePlayer *, CUserCmd *, CMoveData *>(this, player, ucmd, move);
//	}
//
//	void _Update(bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command)
//	{
//		mem::callVirtual<21, void, bool, bool, int, int>(this, received_new_world_update, validframe, incoming_acknowledged, outgoing_command);
//	}
//};
//
//class CPrediction : public IPrediction
//{
//public:
//	EHANDLE m_hLastGround{};
//	bool m_bInPrediction{};
//	bool m_bFirstTimePredicted{};
//	bool m_bOldCLPredictValue{};
//	bool m_bEnginePaused{};
//	int m_nPreviousStartFrame{};
//	int m_nCommandsPredicted{};
//	int m_nServerCommandsAcknowledged{};
//	int m_bPreviousAckHadErrors{};
//	int	m_bPreviousAckErrorTriggersFullLatchReset{};
//	CUtlVector< CHandle< C_BaseEntity > > m_EntsWithPredictionErrorsInLastAck;
//	int m_nIncomingPacketNumber{};
//	float m_flIdealPitch{};
//
//public:
//	void RunCommand(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper)
//	{
//		s::CPrediction_RunCommand.call<void, CPrediction *, C_BasePlayer *, CUserCmd *, IMoveHelper *>(this, player, ucmd, moveHelper);
//	}
//};

class IVRenderView
{
public:
	void SetBlend(float blend)
	{
		mem::callVirtual<4, void, float>(this, blend);
	}

	float GetBlend()
	{
		return mem::callVirtual<5, float>(this);
	}

	void SetColorModulation(float const* blend)
	{
		mem::callVirtual<6, void, float const*>(this, blend);
	}

	void SceneEnd()
	{
		mem::callVirtual<9, void>(this);
	}

	void SetColorModulation(const Color clr)
	{
		const float blend[3]{ static_cast<float>(clr.r) / 255.0f, static_cast<float>(clr.g) / 255.0f, static_cast<float>(clr.b) / 255.0f };

		SetColorModulation(blend);
	}

	void GetColorModulation(float* blend)
	{
		mem::callVirtual<7, void, float*>(this, blend);
	}

	Color GetColorModulation()
	{
		float blend[3]{};
		GetColorModulation(blend);

		return { static_cast<uint8_t>(blend[0] * 255.0f), static_cast<uint8_t>(blend[1] * 255.0f), static_cast<uint8_t>(blend[2] * 255.0f), 255 };
	}

	void GetMatricesForView(const CViewSetup& view, D3DMATRIX* pWorldToView, D3DMATRIX* pViewToProjection, D3DMATRIX* pWorldToProjection, D3DMATRIX* pWorldToPixels)
	{
		mem::callVirtual<50, void, const CViewSetup&, D3DMATRIX*, D3DMATRIX*, D3DMATRIX*, D3DMATRIX*>(this, view, pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels);
	}
};

class IStudioRender
{
public:
	void SetColorModulation(float const* pColor)
	{
		mem::callVirtual<27, void, float const*>(this, pColor);
	}

	void SetColorModulation(const Color clr)
	{
		const float col[3]{ static_cast<float>(clr.r) / 255.0f, static_cast<float>(clr.g) / 255.0f, static_cast<float>(clr.b) / 255.0f };

		SetColorModulation(col);
	}

	void SetAlphaModulation(float flAlpha)
	{
		mem::callVirtual<28, void, float>(this, flAlpha);
	}
};

class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		mem::callVirtual<11, void, int, int, int, int>(this, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		mem::callVirtual<12, void, int, int, int, int>(this, x0, y0, x1, y1);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		mem::callVirtual<14, void, int, int, int, int>(this, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		mem::callVirtual<15, void, int, int, int, int>(this, x0, y0, x1, y1);
	}

	void DrawSetTextFont(HFont font)
	{
		mem::callVirtual<17, void, HFont>(this, font);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		mem::callVirtual<19, void, int, int, int, int>(this, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		mem::callVirtual<20, void, int, int>(this, x, y);
	}

	void DrawPrintText(const wchar_t* text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		mem::callVirtual<22, void, const wchar_t*, int, FontDrawType_t>(this, text, textLen, drawType);
	}

	void DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		mem::callVirtual<23, void, wchar_t, FontDrawType_t>(this, wch, drawType);
	}

	void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload)
	{
		mem::callVirtual<30, void, int, const char*, int, bool>(this, id, filename, hardwareFilter, forceReload);
	}

	void DrawSetTexture(int id)
	{
		mem::callVirtual<32, void, int>(this, id);
	}

	void DrawTexturedRect(int x0, int y0, int x1, int y1)
	{
		mem::callVirtual<34, void, int, int, int, int>(this, x0, y0, x1, y1);
	}

	int CreateNewTextureID(bool procedural)
	{
		return mem::callVirtual<37, int, bool>(this, procedural);
	}

	void SetCursor(HCursor cursor)
	{
		mem::callVirtual<51, void, HCursor>(this, cursor);
	}

	void SetCursorAlwaysVisible(bool visible)
	{
		mem::callVirtual<52, void, bool>(this, visible);
	}

	bool IsCursorVisible()
	{
		return mem::callVirtual<53, bool>(this);
	}

	void UnlockCursor()
	{
		mem::callVirtual<61, void>(this);
	}

	void LockCursor()
	{
		mem::callVirtual<62, void>(this);
	}

	HFont CreateFont()
	{
		return mem::callVirtual<66, HFont>(this);
	}

	bool SetFontGlyphSet(HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0)
	{
		return mem::callVirtual<67, bool, HFont, const char*, int, int, int, int, int, int, int>(this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}

	void surfacePlaySound(const char* fileName)
	{
		return mem::callVirtual<78, void>(this, fileName);
	}

	void GetTextSize(HFont font, const wchar_t* text, int& wide, int& tall)
	{
		mem::callVirtual<75, void, HFont, const wchar_t*, int&, int&>(this, font, text, wide, tall);
	}

	void DrawOutlinedCircle(int x, int y, int radius, int segments)
	{
		mem::callVirtual<99, void, int, int, int, int>(this, x, y, radius, segments);
	}

	void DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1)
	{
		mem::callVirtual<101, void, int, int, int, int, float, float, float, float>(this, x0, y0, x1, y1, texs0, text0, texs1, text1);
	}

	void DrawTexturedPolygon(int n, Vertex_t* pVertice, bool bClipVertices)
	{
		mem::callVirtual<102, void, int, Vertex_t*, bool>(this, n, pVertice, bClipVertices);
	}

	void DrawSetAlphaMultiplier(float alpha)
	{
		mem::callVirtual<108, void, float>(this, alpha);
	}

	float DrawGetAlphaMultiplier()
	{
		return mem::callVirtual<109, float>(this);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
	{
		mem::callVirtual<118, void, int, int, int, int, unsigned int, unsigned int, bool>(this, x0, y0, x1, y1, alpha0, alpha1, bHorizontal);
	}

	void DrawSetTextureRGBAEx(int id, const unsigned char* rgba, int wide, int tall, ImageFormat imageFormat)
	{
		mem::callVirtual<119, void, int, const unsigned char*, int, int, ImageFormat>(this, id, rgba, wide, tall, imageFormat);
	}
};

class IMatSystemSurface : public ISurface
{
public:
	void DisableClipping(bool bDisable)
	{
		mem::callVirtual<156, void, bool>(this, bDisable);
	}

	void GetClippingRect(int& left, int& top, int& right, int& bottom, bool& bClippingDisabled)
	{
		mem::callVirtual<157, void, int&, int&, int&, int&, bool&>(this, left, top, right, bottom, bClippingDisabled);
	}

	void SetClippingRect(int left, int top, int right, int bottom)
	{
		mem::callVirtual<158, void, int, int, int, int>(this, left, top, right, bottom);
	}

	void StartDrawing()
	{
		s::CMatSystemSurface_StartDrawing.call<void>(this);
	}

	void FinishDrawing()
	{
		s::CMatSystemSurface_FinishDrawing.call<void>(this);
	}
};

class IClientModeShared
{
public:
	void* GetViewportAnimationController()
	{
		return mem::callVirtual<9, void*>(this);
	}
};

class IPhysics
{
public:
	IPhysicsEnvironment* CreateEnvironment()
	{
		return mem::callVirtual<5, IPhysicsEnvironment*>(this);
	}
	void DestroyEnvironment(IPhysicsEnvironment* physics_env)
	{
		return mem::callVirtual<6, void>(this, physics_env);
	}
};

class IPhysicsCollision
{
public:
	CPhysCollide* BBoxToCollide(const Vector& mins, const Vector& maxs)
	{
		return mem::callVirtual<29, CPhysCollide*, const Vector&, const Vector&>(this, mins, maxs);
	}
};

class IVDebugOverlay
{
public:
	void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& maxs, const QAngle& angles, int r, int g, int b, int a, float duration)
	{
		mem::callVirtual<1, void, const Vector&, const Vector&, const Vector&, const QAngle&, int, int, int, int, float>(this, origin, mins, maxs, angles, r, g, b, a, duration);
	}

	void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration)
	{
		mem::callVirtual<3, void, const Vector&, const Vector&, int, int, int, bool, float>(this, origin, dest, r, g, b, noDepthTest, duration);
	}

	void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle& angles, int r, int g, int b, int a, float flDuration)
	{
		mem::callVirtual<8, void, const Vector&, const Vector&, const Vector&, const Vector&, const QAngle&, int, int, int, int, float>
			(this, start, end, mins, max, angles, r, g, b, a, flDuration);
	}

	void ScreenPosition(const Vector& point, Vector& screen)
	{
		mem::callVirtual<10, void, const Vector&, Vector&>(this, point, screen);
	}

	void ClearAllOverlays()
	{
		mem::callVirtual<14, void>(this);
	}
};

class CClientState
{
private:
	byte gap0[24]{};
public:
	int m_Socket;
	INetChannelInfo* m_NetChannel{};
	unsigned int m_nChallengeNr{};
	double m_flConnectTime{};
	int m_nRetryNumber{};
	char m_szRetryAddress[260]{};
	CUtlString m_sRetrySourceTag{};
	int m_retryChallenge{};
	int m_nSignonState{};
	double m_flNextCmdTime{};
	int m_nServerCount{};
	unsigned __int64 m_ulGameServerSteamID{};
	int m_nCurrentSequence{};
	CClockDriftMgr m_ClockDriftMgr{};
	int m_nDeltaTick{};
	bool m_bPaused{};
	float m_flPausedExpireTime{};
	int m_nViewEntity{};
	int m_nPlayerSlot{};
	char m_szLevelFileName[128]{};
private:
	byte gap24C[132]{};
public:
	char m_szLevelBaseName[128]{};
private:
	byte gap350[116]{};
public:
	int m_nMaxClients{};
private:
	byte gap3C8[34856]{};
public:
	void* m_StringTableContainer{};
	bool m_bRestrictServerCommands{};
	bool m_bRestrictClientCommands{};
private:
	byte gap8BFC[106]{};
public:
	bool insimulation{};
	int oldtickcount{};
	float m_tickRemainder{};
	float m_frameTime{};
	int lastoutgoingcommand{};
	int chokedcommands{};
	int last_command_ack{};
	int command_ack{};
	int m_nSoundSequence{};
	bool ishltv{};
	bool isreplay{};
private:
	byte gap8C8A[278]{};
public:
	int demonum{};
	CUtlString demos[32]{};
private:
	byte gap8EE8[344184]{};
public:
	bool m_bMarkedCRCsUnverified{};

public:
	inline bool IsConnected() const
	{
		return m_nSignonState >= SIGNONSTATE_CONNECTED;
	}

	inline bool IsActive() const
	{
		return m_nSignonState == SIGNONSTATE_FULL;
	}

	inline bool IsPaused()
	{
		return s::CClientState_IsPaused.call<bool>(this);
	}

	inline void ForceFullUpdate()
	{
		s::CClientState_ForceFullUpdate.call<void>(this);
	}

public:
	float GetClientInterpAmount()
	{
		return s::CClientState_GetClientInterpAmount.call<float>(this);
	}

	void SendStringCmd(const char* command)
	{
		s::CBaseClientState_SendStringCmd.call<void>(this, command);
	}
};

class IDemoRecorder
{
public:
	bool IsRecording()
	{
		return mem::callVirtual<4, bool>(this);
	}

	bool RecordUserInput(int command_number)
	{
		return mem::callVirtual<9, bool>(this, command_number);
	}
};


class IDemoPlayer
{
public:
	bool IsPlayingBack()
	{
		return mem::callVirtual<6, bool>(this);
	}
};

class IViewRenderBeams
{
public:
	void DrawBeam(Beam_t* pbeam)
	{
		mem::callVirtual<4, void, Beam_t*>(this, pbeam);
	}

	Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo)
	{
		return mem::callVirtual<12, Beam_t*, BeamInfo_t&>(this, beamInfo);
	}
};


class IGameEventManager2
{
public:
	bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide)
	{
		return mem::callVirtual<3, bool, IGameEventListener2*, const char*, bool>(this, listener, name, bServerSide);
	}
	bool FindListener(IGameEventListener2* listener, const char* name)
	{
		return mem::callVirtual<4, bool, IGameEventListener2*, const char*>(this, listener, name);
	}
	bool RemoveListener(IGameEventListener2* listener)
	{
		return mem::callVirtual<5, bool, IGameEventListener2*>(this, listener);
	}
	IGameEvent* CreateNewEvent(const char* name, bool force) // actually called CreateEvent but there's some macro called that too
	{
		return mem::callVirtual<6, IGameEvent*, const char*, bool>(this, name, force);
	}

	bool FireEventClientSide(IGameEvent* event)
	{
		return mem::callVirtual<8, bool, IGameEvent*>(this, event);
	}
};

class IGameEventListener2 : public IGameEventManager2
{
public:
	virtual	~IGameEventListener2(void) {};

	// FireEvent is called by EventManager if event just occurred
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(IGameEvent* event) = 0;
};

class IMemAlloc
{
public:
	void* Alloc(size_t nSize)
	{
		return mem::callVirtual<1, void*>(this, nSize);
	}

	void* Realloc(void* pMem, size_t nSize)
	{
		return mem::callVirtual<3, void*>(this, pMem, nSize);
	}

	void Free(void* pMem)
	{
		return mem::callVirtual<5, void>(this, pMem);
	}
};

class IPredictableList
{
public:
	C_BaseEntity* GetPredictable(int slot)
	{
		return mem::callVirtual<0, C_BaseEntity*>(this, slot);
	}

	int GetPredictableCount()
	{
		return mem::callVirtual<1, int>(this);
	}
};

class IPredictionSystem
{
public:
	IPredictionSystem* g_pPredictionSystems;
	IPredictionSystem* m_pNextSystem;
	bool				m_bSuppressEvent;
	C_BaseEntity* m_pSuppressHost;
	int					m_nStatusPushed;
};

class IFileSystem
{
public:

};

class ISteamFriends
{
public:
	void ActivateGameOverlayToWebPage(const char* pchURL)
	{
		mem::callVirtual<30, void, const char*>(this, pchURL);
	}

	bool SetRichPresence(const char* pchKey, const char* pchValue)
	{
		return mem::callVirtual<43, bool, const char*, const char*>(this, pchKey, pchValue);
	}
	virtual void ClearRichPresence()
	{
		return mem::callVirtual<44, void>(this);
	}

};

class CSteamAPIContext
{
public:
	ISteamFriends* SteamFriends()
	{
		return *reinterpret_cast<ISteamFriends**>(reinterpret_cast<uintptr_t>(this) + 16);
	}

};

class IMDLCache {
public:
	void BeginLock() {
		return mem::callVirtual<25, void>(this);
	};
	void EndLock() {
		return mem::callVirtual<26, void>(this);
	};
};

MAKE_IFACE(IUniformRandomStream, UniformRandomStream, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? F3 0F 59 CA 44 8D 42").fixRip(3).deref());
MAKE_IFACE(IBaseClientDLL, client, mem::findInterface("client.dll", "VClient017"));
MAKE_IFACE(c_engine_effects, e_effects, mem::findInterface("engine.dll", "VEngineEffects001"));
MAKE_IFACE(IClientEntityList, ent_list, mem::findInterface("client.dll", "VClientEntityList003"));
MAKE_IFACE(ICvar, cvar, mem::findInterface("vstdlib.dll", "VEngineCvar004"));
MAKE_IFACE(IVEngineClient, engine, mem::findInterface("engine.dll", "VEngineClient014"));
MAKE_IFACE(IVEngineServer, engine_server, mem::findInterface("engine.dll", "VEngineServer023"));
MAKE_IFACE(IEngineSound, sound_engine, mem::findInterface("engine.dll", "IEngineSoundClient003"));
MAKE_IFACE(IEngineTrace, trace, mem::findInterface("engine.dll", "EngineTraceClient003"));
MAKE_IFACE(IEngineVGui, vgui, mem::findInterface("engine.dll", "VEngineVGui002"));
MAKE_IFACE(IGameMovement, game_movement, mem::findInterface("client.dll", "GameMovement001"));
MAKE_IFACE(CGlobalVarsBase, global_vars, mem::findBytes("engine.dll", "48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B CA").fixRip(3));
MAKE_IFACE(IInput, input, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? 48 8B 01 FF 90 ? ? ? ? 85 C0 0F 84 ? ? ? ? F3 0F 10 05").fixRip(3).deref());
MAKE_IFACE(IInputSystem, input_system, mem::findInterface("inputsystem.dll", "InputSystemVersion001"));
MAKE_IFACE(IMaterialSystem, mat_sys, mem::findInterface("materialsystem.dll", "VMaterialSystem082")); // >>
MAKE_IFACE(IVModelInfo, model_info, mem::findInterface("engine.dll", "VModelInfoClient006"));
MAKE_IFACE(IVModelRender, model_render, mem::findInterface("engine.dll", "VEngineModel016"));
MAKE_IFACE(IMoveHelper, move_helper, mem::findBytes("client.dll", "48 8B 05 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8B 51 10").fixRip(3).deref());
MAKE_IFACE(CPrediction, pred, mem::findInterface("client.dll", "VClientPrediction001"));
MAKE_IFACE(IVRenderView, render_view, mem::findInterface("engine.dll", "VEngineRenderView014"));
MAKE_IFACE(IStudioRender, studio_render, mem::findInterface("studiorender.dll", "VStudioRender025"));
MAKE_IFACE(IMatSystemSurface, surface, mem::findInterface("vguimatsurface.dll", "VGUI_Surface030"));
MAKE_IFACE(IClientModeShared, client_mode, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? 48 8B 10 48 8B 19 48 8B C8 FF 92").fixRip(3).deref());
MAKE_IFACE(IPhysics, physics, mem::findInterface("vphysics.dll", "VPhysics031"));
MAKE_IFACE(IPhysicsCollision, physics_col, mem::findInterface("vphysics.dll", "VPhysicsCollision007"));
MAKE_IFACE(IVDebugOverlay, debug_overlay, mem::findInterface("engine.dll", "VDebugOverlay003"));
MAKE_IFACE(CClientState, client_state, mem::findBytes("engine.dll", "48 8D 0D ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 48 89 6C 24").fixRip(3));
MAKE_IFACE(IDemoRecorder, demo_recorder, mem::findBytes("engine.dll", "48 8B 0D ? ? ? ? 8D 56").fixRip(3).deref());
MAKE_IFACE(IDemoPlayer, demo_player, mem::findBytes("engine.dll", "48 8B 0D ? ? ? ? 40 B7").fixRip(3).deref());
MAKE_IFACE(IViewRenderBeams, beams, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? 48 8B 01 FF 10 48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 48 8D 8B").fixRip(3).deref());
MAKE_IFACE(IGameEventManager2, event_manager, mem::findInterface("engine.dll", "GAMEEVENTSMANAGER002"));
MAKE_IFACE(IMemAlloc, mem_alloc, mem::findExport("tier0.dll", "g_pMemAlloc").deref());
MAKE_IFACE(IPredictableList, predictables, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? 48 8B 01 FF 50 08 33 DB 8B F0 85 C0 7E 37 0F 1F 40 00").fixRip(3).deref());
MAKE_IFACE(IPredictionSystem, pred_system, mem::findBytes("client.dll", "48 89 05 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 05 ? ? ? ? E9 ? ? ? ? CC CC CC CC CC CC CC CC 48 83 EC 28").fixRip(3));
MAKE_IFACE(IFileSystem, file_system, mem::findBytes("client.dll", "48 8B 0D ? ? ? ? 4C 8D 05 ? ? ? ? 48 83 C1 ? 48 8D 94 24 ? ? ? ? 45 33 C9").fixRip(3).deref());
MAKE_IFACE(CSteamAPIContext, steam_api_context, mem::findBytes("client.dll", "48 8B 05 ? ? ? ? 48 8B 48 30 48 85 C9 74 06").fixRip(3).deref());
MAKE_IFACE(IDirect3DDevice9, dxdev, mem::findBytes("shaderapidx9.dll", "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 FF 50 ? 48 C7 05").fixRip(3).deref());
//MAKE_IFACE_SIG(IViewRenderBeams, RenderBeams, "client.dll", "48 8B 0D ? ? ? ? 48 8B 01 FF 10 48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 48 8D 8B", 0, 1, true);
MAKE_IFACE(IMDLCache, mdl_cache, mem::findInterface("datacache.dll", "MDLCache004"));