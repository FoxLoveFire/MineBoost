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

#pragma once

#include "cpp_api/s_base.h"
#include <vector>
#include <string>

class ScriptApiFunctionFunctions
{
public:
	ScriptApiFunctionFunctions(const std::string &name, const std::string &setting);
	ScriptApiFunctionFunctions(const std::string &name, const int &function);
	std::string m_name;
	bool is_enabled();
	void toggle(lua_State *L, int error_handler);

private:
	std::string m_setting;
	int m_function_ref;
};

class ScriptApiFunctionsCategory
{
public:
	ScriptApiFunctionsCategory(const std::string &name);
	~ScriptApiFunctionsCategory();
	std::string m_name;
	void read_functions(lua_State *L);
	std::vector<ScriptApiFunctionFunctions *> m_function;
};

class ScriptApiFunctions : virtual public ScriptApiBase
{
public:
	virtual ~ScriptApiFunctions();
	void init_functions();
	void toggle_func(ScriptApiFunctionFunctions *function);
	bool m_function_loaded = false;
	std::vector<ScriptApiFunctionsCategory *> m_function_categories;
};
