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

/* ObjectData/String.cpp
**
** Defines the ObjectData_String functions.
*/

#include "../ObjectData.hpp"

#include "../ObjectExpression.hpp"
#include "../object_io.hpp"

#include <map>
#include <sstream>



typedef std::map<std::string, ObjectData_String> StringTable;

static StringTable _string_table;



std::string const & ObjectData_String::add(std::string const & string)
{
	if (_string_table.count(string))
		return _string_table[string].name;

	ObjectData_String & data(_string_table[string]);

	data.string = string + '\0';

	std::ostringstream oss;
	oss << "__string_" << ObjectExpression::get_filename() << "_" << _string_table.size();
	data.name = oss.str();

	ObjectExpression::add_symbol(data.name, ObjectExpression::ET_INT);

	return data.name;
}

void ObjectData_String::generate_symbols()
{
	ObjectExpression::Pointer expr;
	StringTable::iterator it;
	bigsint i = 0;

	for (it = _string_table.begin(); it != _string_table.end(); ++it)
	{
		expr = ObjectExpression::create_value_int(i++, SourcePosition::none());
		ObjectExpression::add_symbol(it->second.name, expr);
	}
}

void ObjectData_String::iterate(IterFunc iterFunc, std::ostream * out)
{
	StringTable::iterator it;

	for (it = _string_table.begin(); it != _string_table.end(); ++it)
		iterFunc(out, it->second);
}

void ObjectData_String::read_objects(std::istream * in)
{
	read_object(in, &_string_table);
}

void ObjectData_String::write_objects(std::ostream * out)
{
	write_object(out, _string_table);
}


