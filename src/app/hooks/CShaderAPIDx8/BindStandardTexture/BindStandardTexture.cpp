#include "../../../hooks.hpp"


MAKE_HOOK(
	CShaderAPIDx8_BindStandardTexture,
	s::CShaderAPIDx8_BindStandardTexture.get(),
	void,
	void* rcx, Sampler_t sampler, StandardTextureId_t id)
{
	if (cfg::spritecard_texture_hack && reinterpret_cast<uintptr_t>(_ReturnAddress()) == s::BindStandardTexture_spritecard_call.get()) {
		CALL_ORIGINAL(rcx, SHADER_SAMPLER0, TEXTURE_WHITE);
		return;
	}

	CALL_ORIGINAL(rcx, sampler, id);
}
