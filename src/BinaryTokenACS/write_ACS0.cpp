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

/* BinaryTokenACS/write_ACS0.cpp
**
** Defines the BinaryTokenACS::write_ACS0_* functions.
*/

#include "../BinaryTokenACS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"



static bigsint _script_count = 0;
static bigsint _string_count = 0;
static bigsint _string_offset = 0;



void BinaryTokenACS::write_ACS0_8(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
}
void BinaryTokenACS::write_ACS0_16(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
	out->put(char((i >> 8) & 0xFF));
}
void BinaryTokenACS::write_ACS0_32(std::ostream * out, ObjectExpression const & expr)
{
	switch (expr.getType())
	{
	case ObjectExpression::ET_FLOAT:
		write_ACS0_32(out, (bigsint)(expr.resolveFloat() * 65536.0));
		break;

	case ObjectExpression::ET_INT:
		write_ACS0_32(out, expr.resolveInt());
		break;

	default:
		throw SourceException("invalid ET", SourcePosition::none, "BinaryTokenACS::write_ACS0_32");
	}
}
void BinaryTokenACS::write_ACS0_32(std::ostream * out, bigsint i)
{
	out->put(char((i >>  0) & 0xFF));
	out->put(char((i >>  8) & 0xFF));
	out->put(char((i >> 16) & 0xFF));
	out->put(char((i >> 24) & 0xFF));
}

void BinaryTokenACS::write_ACS0_script(std::ostream * out, ObjectData_Script const & s)
{
	write_ACS0_32(out, (s.stype * 1000) + s.number);
	write_ACS0_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
	write_ACS0_32(out, s.argCount <= 3 ? s.argCount : 3);
}

void BinaryTokenACS::write_ACS0_script_count(std::ostream * out)
{
	write_ACS0_32(out, _script_count);
}

void BinaryTokenACS::write_ACS0_script_counter(std::ostream * out, ObjectData_Script const & s)
{
	++_script_count;
}

void BinaryTokenACS::write_ACS0_string(std::ostream * out, ObjectData_String const & s)
{
	*out << s.string;
}

void BinaryTokenACS::write_ACS0_string_count(std::ostream * out)
{
	write_ACS0_32(out, _string_count);
}

void BinaryTokenACS::write_ACS0_string_counter(std::ostream * out, ObjectData_String const & s)
{
	++_string_count;
}

void BinaryTokenACS::write_ACS0_string_offset(std::ostream * out, ObjectData_String const & s)
{
	bigsint base_offset =
		ObjectExpression::get_address_count() +
		4 + (_script_count * 12) +
		4 + (_string_count * 4);

	write_ACS0_32(out, base_offset + _string_offset);

	_string_offset += s.string.size();
}


