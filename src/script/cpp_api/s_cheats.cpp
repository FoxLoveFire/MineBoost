/*
Dragonfire
Copyright (C) 2020 Elias Fleckenstein <eliasfleckenstein@web.de>

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

#include "cpp_api/s_cheats.h"
#include "cpp_api/s_base.h"
#include "cpp_api/s_internal.h"
#include "settings.h"

ScriptApiFunctionFunctions::ScriptApiFunctionFunctions(
		const std::string &name, const std::string &setting) :
		m_name(name),
		m_setting(setting), m_function_ref(0)
{
}

ScriptApiFunctionFunctions::ScriptApiFunctionFunctions(const std::string &name, const int &function) :
		m_name(name), m_setting(""), m_function_ref(function)
{
}

bool ScriptApiFunctionFunctions::is_enabled()
{
	try {
		return !m_function_ref && g_settings->getBool(m_setting);
	} catch (SettingNotFoundException &) {
		return false;
	}
}

void ScriptApiFunctionFunctions::toggle(lua_State *L, int error_handler)
{
	if (m_function_ref) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, m_function_ref);
		lua_pcall(L, 0, 0, error_handler);
	} else
		g_settings->setBool(m_setting, !is_enabled());
}

ScriptApiFunctionsCategory::ScriptApiFunctionsCategory(const std::string &name) : m_name(name)
{
}

ScriptApiFunctionsCategory::~ScriptApiFunctionsCategory()
{
	for (auto i = m_function.begin(); i != m_function.end(); i++)
		delete *i;
}

void ScriptApiFunctionsCategory::read_functions(lua_State *L)
{
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		ScriptApiFunctionFunctions *function = nullptr;
		std::string name = lua_tostring(L, -2);
		if (lua_isstring(L, -1))
			function = new ScriptApiFunctionFunctions(name, lua_tostring(L, -1));
		else if (lua_isfunction(L, -1)) {
			function = new ScriptApiFunctionFunctions(
					name, luaL_ref(L, LUA_REGISTRYINDEX));
			lua_pushnil(L);
		}
		if (function)
			m_function.push_back(function);
		lua_pop(L, 1);
	}
}

ScriptApiFunctions::~ScriptApiFunctions()
{
	for (auto i = m_function_categories.begin(); i != m_function_categories.end(); i++)
		delete *i;
}

void ScriptApiFunctions::init_functions()
{
	SCRIPTAPI_PRECHECKHEADER

	lua_getglobal(L, "core");
	lua_getfield(L, -1, "functions");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 2);
		return;
	}
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (lua_istable(L, -1)) {
			ScriptApiFunctionsCategory *category =
					new ScriptApiFunctionsCategory(lua_tostring(L, -2));
			category->read_functions(L);
			m_function_categories.push_back(category);
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 2);
	m_function_loaded = true;
}

void ScriptApiFunctions::toggle_func(ScriptApiFunctionFunctions *function)
{
	SCRIPTAPI_PRECHECKHEADER

	PUSH_ERROR_HANDLER(L);
	int error_handler = lua_gettop(L) - 1;
	lua_insert(L, error_handler);

	function->toggle(L, error_handler);
}
