#include "../../../hooks.hpp"

MAKE_HOOK(
	CHLClient_FrameStageNotify,
	s::CHLClient_FrameStageNotify.get(),
	void,
	void* rcx, ClientFrameStage_t curStage)
{
	if (curStage == FRAME_NET_UPDATE_START) {
		i::engine->FireEvents();
	}

	if (curStage == FRAME_RENDER_START) {
		input->processStart();
	}

	if (curStage == FRAME_RENDER_END) {
		input->processEnd();
	}

	CALL_ORIGINAL(rcx, curStage);

	class_ids::init();

	if (curStage == FRAME_NET_UPDATE_END) {
		ec->update();
		player_data->update();
		lag_comp->update();
	}

	if (curStage == FRAME_RENDER_START)
	{
		mat_mod->run();

		draw->updateW2S();

		esp->run();		
		misc->paintFOVCircle();

		draw->string(fonts::font_esp, vec2(5, 5), "pipeline [dev]", Color(255, 255, 255, 255), 0);

		//crit_manager->paint();
		//tick_base->paint();
		//spectator_list->run();
		
		draw->swap_commands();
	}
}