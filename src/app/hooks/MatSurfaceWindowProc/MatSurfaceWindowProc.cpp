#include "../../hooks.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

MAKE_HOOK(
	MatSurfaceWindowProc,
	s::MatSurfaceWindowProc.get(),
	LRESULT,
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwnd_tf{ FindWindowA(0, "Team Fortress 2 - Direct3D 9 - 64 Bit") };

	//s: I was having a freeze when gui was open and I opened some other window (in win), this fixes that
	if (GetFocus() != hwnd_tf) {
		return CALL_ORIGINAL(hwnd, uMsg, wParam, lParam);
	}

	input->wndproc(uMsg, wParam, lParam);
	input_tick->wndproc(uMsg, wParam, lParam);


	if (menu->isOpen()) {
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		if (ImGui::GetIO().WantTextInput)
		{
			i::input_system->ResetInputState();
			return 1;
		}

		if (uMsg >= WM_MOUSEFIRST && WM_MOUSELAST >= uMsg)
			return 1;
	}

	return CALL_ORIGINAL(hwnd, uMsg, wParam, lParam);
}
