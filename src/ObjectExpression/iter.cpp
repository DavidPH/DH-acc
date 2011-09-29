/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* ObjectExpression/iter.cpp
**
** Defines the ObjectExpression::iter_* functions.
*/

#include "../ObjectExpression.hpp"



void ObjectExpression::iter_function(void (*iter)(ObjectData_Function const &))
{
	for (std::map<std::string, ObjectData_Function>::iterator it(_function_table.begin()); it != _function_table.end(); ++it)
		iter(it->second);
}
void ObjectExpression::iter_function(void (*iter)(std::ostream *, ObjectData_Function const &), std::ostream * out)
{
	for (std::map<std::string, ObjectData_Function>::iterator it(_function_table.begin()); it != _function_table.end(); ++it)
		iter(out, it->second);
}
void ObjectExpression::iter_registerarray_map(void (*iter)(std::ostream *, ObjectData_RegisterArray const &), std::ostream * out)
{
	for (std::map<std::string, ObjectData_RegisterArray>::iterator it(_registerarray_map_table.begin()); it != _registerarray_map_table.end(); ++it)
		iter(out, it->second);
}
void ObjectExpression::iter_script(void (*iter)(std::ostream *, ObjectData_Script const &), std::ostream * out)
{
	for (std::map<std::string, ObjectData_Script>::iterator it(_script_table.begin()); it != _script_table.end(); ++it)
		iter(out, it->second);
}
void ObjectExpression::iter_string(void (*iter)(std::ostream *, ObjectData_String const &), std::ostream * out)
{
	for (std::vector<ObjectData_String>::iterator it(_string_table.begin()); it != _string_table.end(); ++it)
		iter(out, *it);
}


