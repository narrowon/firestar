#pragma once
#include "../../../game/game.hpp"

class Menu final /*: public HasLoad, public HasUnload*/
{
private:
	bool m_open{};

public:
	/*bool onLoad() override;
	bool onUnload() override;*/

public:
	void run();
	int tab = 0;
	int sub = 0;

	ImColor menu_accent = ImColor(255, 114, 142);;
	ImColor menu_subaccent = ImColor(255, 170, 173);
public:
	bool isOpen() const
	{
		return m_open;
	}
};

MAKE_UNIQUE(Menu, menu);