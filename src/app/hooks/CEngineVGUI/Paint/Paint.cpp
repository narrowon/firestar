#include "../../../hooks.hpp"

MAKE_HOOK(
	CEngineVGui_Paint,
	s::CEngineVGui_Paint.get(),
	void,
	void* rcx, PaintMode_t mode)
{
	if (mode & PAINT_INGAMEPANELS)
	{
		misc->prediction_path();

	}

	CALL_ORIGINAL(rcx, mode);

	if (mode & PAINT_UIPANELS)
	{
		if (i::steam_api_context && i::steam_api_context->SteamFriends()) {

			i::steam_api_context->SteamFriends()->SetRichPresence("steam_display", "#TF_RichPresence_Display");
			i::steam_api_context->SteamFriends()->SetRichPresence("state", "PlayingMatchGroup");
			i::steam_api_context->SteamFriends()->SetRichPresence("currentmap", "PIPELINE");
		}
	}
}