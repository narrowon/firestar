#include "../../hooks.hpp"


MAKE_HOOK(
	NotificationQueue_Add,
	s::NotificationQueue_Add.get(),
	int,
	CEconNotification* pNotification)
{
	if (cfg::misc_remove_item_found_notification && pNotification)
	{
		const std::string_view message{ pNotification->m_pText };

		if (!message.empty() && message.compare("TF_HasNewItems") == 0)
		{
			pNotification->Accept();
			pNotification->Trigger();
			pNotification->UpdateTick();
			pNotification->MarkForDeletion();

			return 0;
		}

	//	i::cvar->ConsoleColorPrintf({ 255, 255, 255, 255 }, "NotificationQueue_Add: %s\n", message.data());
	}

	return CALL_ORIGINAL(pNotification);
}