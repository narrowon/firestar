#include "../../../hooks.hpp"
#include <d3d9.h>


MAKE_HOOK(Direct3DDevice9_Present, mem::findVirtual(i::dxdev, 17).get(), HRESULT, IDirect3DDevice9* device, const RECT* srcRect, const RECT* destRect, HWND destWindowOverride, const RGNDATA* dirtyRegion) {
	draw->initialize(device);
	draw->setup_states(device);
	draw->start();

	/*	ImGui::Begin("aaaa");
		ImGui::End();*/

//	if (Vars::Menu::IsOpen.Value)
		//F::Menu.Run();

	//if (menu->isOpen())
		menu->run();

	auto* list = ImGui::GetBackgroundDrawList();
	draw->render(list);

	tick_base->paint();
	crits->paint();
	spectator_list->run();

	draw->end();
	draw->backup_states(device);

	return CALL_ORIGINAL(device, srcRect, destRect, destWindowOverride, dirtyRegion);
}	 