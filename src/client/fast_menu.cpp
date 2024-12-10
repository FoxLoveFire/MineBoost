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

#include "script/scripting_client.h"
#include "client/gameui.h"
#include "client/client.h"
#include "client/fontengine.h"
#include "fast_menu.h"
#include <cstddef>

FontMode RenderMenu::fontStringToEnum(std::string str)
{
	if (str == "FM_Standard")
		return FM_Standard;
	else if (str == "FM_Mono")
		return FM_Mono;
	else if (str == "FM_Fallback")
		return _FM_Fallback;
	else if (str == "FM_MaxMode")
		return FM_MaxMode;
	else if (str == "FM_Unspecified")
		return FM_Unspecified;
	else
		return FM_Standard;
}

std::string checkPrivCustom(std::string &v)
{
	if (v == "Fly") {
		return "fly";
	} else if (v == "Fast Move") {
		return "fast";
	} else if (v == "Noclip") {
		return "noclip";
	} else if (v == "Hitboxes") {
		return "debug";
	}
	
	return "";
}

struct settings{
	v3f bg_color, active_bg_color, font_color, selected_font_color;
	std::vector<v3f> sets = {bg_color, active_bg_color, font_color, selected_font_color};
};

RenderMenu::RenderMenu(Client *client) : m_client(client)
{	
	// Настройка для загрузки шрифта 
	FontMode fontMode = fontStringToEnum(g_settings->get("menu_font"));
	
	// Setting from struct settings
	settings menusets;
	
	menusets.sets[0] = g_settings->getV3F("menu_bg_color");
	menusets.sets[1] = g_settings->getV3F("menu_active_bg_color");
	menusets.sets[2] = g_settings->getV3F("menu_font_color");
	menusets.sets[3] = g_settings->getV3F("menu_selected_font_color");


	m_bg_color = video::SColor(g_settings->getU32("menu_bg_color_alpha"),
			menusets.sets[0].X, menusets.sets[0].Y, menusets.sets[0].Z);

	m_active_bg_color = video::SColor(
			g_settings->getU32("menu_active_bg_color_alpha"),
			menusets.sets[1].X, menusets.sets[1].Y, menusets.sets[1].Z);

	m_font_color = video::SColor(g_settings->getU32("menu_font_color_alpha"),
			menusets.sets[2].X, menusets.sets[2].Y, menusets.sets[2].Z);

	m_selected_font_color = video::SColor(
			g_settings->getU32("menu_selected_font_color_alpha"),
			menusets.sets[3].X, menusets.sets[3].Y, menusets.sets[3].Z);

	m_head_height = g_settings->getU32("menu_head_height");
	m_entry_height = g_settings->getU32("menu_entry_height");
	m_entry_width = g_settings->getU32("menu_entry_width");

	m_font = g_fontengine->getFont(FONT_SIZE_UNSPECIFIED, fontMode);

	if (!m_font) {
		errorstream << "Fast menu: Unable to load font" << std::endl;
	} else {
		core::dimension2d<u32> dim = m_font->getDimension(L"M");
		m_fontsize = v2u32(dim.Width, dim.Height);
		m_font->grab();
	}
	
	
	m_fontsize.X = MYMAX(m_fontsize.X, 1);
	m_fontsize.Y = MYMAX(m_fontsize.Y, 1);
}

void RenderMenu::drawEntry(video::IVideoDriver *driver, std::string name, int number,
		bool selected, bool active, RenderMenuType entry_type)
{
	int x = m_gap, y = m_gap, width = m_entry_width, height = m_entry_height;
	
	video::SColor *bgcolor = &m_bg_color, *fontcolor = &m_font_color;
	
	if (entry_type == HEAD) {
		bgcolor = &m_active_bg_color;
		height = m_head_height;
	} else {
		
		bool is_category = entry_type == CATEGORY;
		
		y += m_gap + m_head_height +
		     (number + (is_category ? 0 : m_selected_category)) *
				     (m_entry_height + m_gap);
				     
		x += (is_category ? 0 : m_gap + m_entry_width);
		
		if (active)
			bgcolor = &m_active_bg_color;
			
		if (selected)
			fontcolor = &m_selected_font_color;
	}
	
	driver->draw2DRectangle(*bgcolor, core::rect<s32>(x, y, x + width, y + height));
	
	if (selected)
		driver->draw2DRectangleOutline( core::rect<s32>(x - 1, y - 1, x + width, y + height), *fontcolor);
				
	int fx = x + 5, fy = y + (height - m_fontsize.Y) / 2;
	
	core::rect<s32> fontbounds(fx, fy, fx + m_fontsize.X * name.size(), fy + m_fontsize.Y);
	
	m_font->draw(name.c_str(), fontbounds, *fontcolor, false, false);
}

void RenderMenu::draw(video::IVideoDriver *driver)
{
	GET_SCRIPTPTR

	drawEntry(driver, "MineBoost", 0, false, false, HEAD);
				
	int category_count = 0;
	
	for (auto category = script->m_function_categories.begin(); category != script->m_function_categories.end(); category++) {
		bool is_selected = category_count == m_selected_category;
		
			drawEntry(driver, (*category)->m_name, category_count, is_selected, false,
					CATEGORY);
					
		if (is_selected && m_function_layer) {
			int function_count = 0;
			
			for (auto function = (*category)->m_function.begin(); function != (*category)->m_function.end(); function++) {
				drawEntry(driver, (*function)->m_name, function_count, function_count == m_selected_function, (*function)->is_enabled());
				function_count++;
			}
		}
		category_count++;
	}
}

void RenderMenu::drawHUD(video::IVideoDriver *driver, double dtime)
{
	GET_SCRIPTPTR

	m_rainbow_offset += dtime;

	m_rainbow_offset = fmod(m_rainbow_offset, 6.0f);

	std::vector<std::string> enabled_functions;

	enabled_functions.clear();
	int function_count = 0;
	
	auto category = script->m_function_categories;
	for (auto category = script->m_function_categories.begin(); category != script->m_function_categories.end(); category++) {
		for (auto function = (*category)->m_function.begin(); function != (*category)->m_function.end(); function++) {
			if ((*function)->is_enabled()) {
				if ((*function) -> m_name == "Fly" || (*function) -> m_name == "Noclip" || (*function) -> m_name == "Fast Move" || (*function)->m_name == "Hitboxes") {
					if (m_client->checkPrivilege(checkPrivCustom((*function)->m_name))) {
						enabled_functions.push_back((*function)->m_name + " [Privilege]");
					} else {
						enabled_functions.push_back((*function)->m_name + " [No privilege]");
					}
				 } else {
					enabled_functions.push_back((*function)->m_name + " [Core]");
				 }
				function_count++;
			}
		}
	}

	if (enabled_functions.empty())
		return;

	std::vector<video::SColor> colors;

	for (int i = 0; i < function_count; i++) {
		video::SColor color = video::SColor(255, 0, 0, 0);
		f32 h = (f32)i * 2.0f / (f32)function_count - m_rainbow_offset;
		if (h < 0)
			h = 6.0f + h;
		f32 x = (1 - fabs(fmod(h, 2.0f) - 1.0f)) * 255.0f;
		switch ((int)h) {
		case 0:
			color = video::SColor(255, 255, x, 0);
			break;
		case 1:
			color = video::SColor(255, x, 255, 0);
			break;
		case 2:
			color = video::SColor(255, 0, 255, x);
			break;
		case 3:
			color = video::SColor(255, 0, x, 255);
			break;
		case 4:
			color = video::SColor(255, x, 0, 255);
			break;
		case 5:
			color = video::SColor(255, 255, 0, x);
			break;
		}
		colors.push_back(color);
	}

	core::dimension2d<u32> screensize = driver->getScreenSize();

	u32 y = 5;

	int i = 0;
	for (std::string func : enabled_functions) {
		core::dimension2d<u32> dim =
				m_font->getDimension(utf8_to_wide(func).c_str());
		u32 x = screensize.Width - 5 - dim.Width;

		core::rect<s32> fontbounds(x, y, x + dim.Width, y + dim.Height);
		m_font->draw(func.c_str(), fontbounds, colors[i], false, false);

		y += dim.Height;
		i++;
	}
}

void RenderMenu::selectUp()
{
	GET_SCRIPTPTR

	int max = (m_function_layer ? script->m_function_categories[m_selected_category] ->m_function.size()
				 : script->m_function_categories.size()) - 1;
	
	int *selected = m_function_layer ? &m_selected_function : &m_selected_category;
	--*selected; 
	
	if (*selected < 0)
		*selected = max;
}

void RenderMenu::selectDown()
{
	GET_SCRIPTPTR

	int max = (m_function_layer ? script->m_function_categories[m_selected_category]
								  ->m_function.size()
				 : script->m_function_categories.size()) -
		  1;
	int *selected = m_function_layer ? &m_selected_function : &m_selected_category;
	++*selected;
	if (*selected > max)
		*selected = 0;
}

void RenderMenu::selectRight()
{
	if (m_function_layer)
		return;
	m_function_layer = true;
	m_selected_function = 0;
}

void RenderMenu::selectLeft()
{
	if (!m_function_layer)
		return;
	m_function_layer = false;
}

void RenderMenu::selectConfirm()
{
	GET_SCRIPTPTR

	if (m_function_layer)
		script->toggle_func(script->m_function_categories[m_selected_category]
						     ->m_function[m_selected_function]);
}
