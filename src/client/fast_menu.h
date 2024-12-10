/*
MineBoost
Copyright (C) 2020 Elias Fleckenstein <eliasfleckenstein@web.de>
Copyright (C) 2023 Shkatov Ivan <ivanskatov672@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#pragma once

#include "client/client.h"
#include "irrlichttypes_extrabloated.h"
#include "script/scripting_client.h"
#include <cstddef>
#include <string>

#define GET_SCRIPTPTR                                                         			\
	ClientScripting *script = m_client->getScript();                                 \
	if (!script || !script->m_function_loaded)                                         \
		return;

enum RenderMenuType{
	
	HEAD,
	CATEGORY,
	ENTRY,
};

class RenderMenu{
private:

	bool m_function_layer = false;
	int m_selected_function = 0;
	int m_selected_category = 0;

	int m_head_height = 50;
	int m_entry_height = 40;
	int m_entry_width = 200;
	int m_gap = 5;
	
	video::SColor m_bg_color = video::SColor(192, 255, 145, 88);
	video::SColor m_active_bg_color = video::SColor(192, 255, 87, 53);
	video::SColor m_font_color = video::SColor(255, 0, 0, 0);
	video::SColor m_selected_font_color = video::SColor(255, 255, 252, 88);

	FontMode fontStringToEnum(std::string str);

	Client *m_client;

	gui::IGUIFont *m_font = nullptr;
	v2u32 m_fontsize;

	float m_rainbow_offset = 0.0;

public:
	RenderMenu(Client *client);
	
	ClientScripting *getScript(){ return m_client->getScript(); };
	
	void draw(video::IVideoDriver *driver);

	void drawHUD(video::IVideoDriver *driver, double dtime);

	void drawEntry(video::IVideoDriver *driver, std::string name, int number,
			bool selected, bool active,
			RenderMenuType entry_type = ENTRY);

	void selectUp();
	void selectDown();
	void selectLeft();
	void selectRight();
	void selectConfirm();
};
