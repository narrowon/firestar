#include "../../../hooks.hpp"

MAKE_HOOK(Direct3DDevice9_Reset, mem::findVirtual(i::dxdev, 16).get(), HRESULT,
	LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const HRESULT Original = CALL_ORIGINAL(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	return Original;
}