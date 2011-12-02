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

/* BinaryTokenZDACS/write_ACSE.cpp
**
** Defines the BinaryTokenZDACS::write_ACSE_* functions.
*/

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../ost_type.hpp"

#include <sstream>



void BinaryTokenZDACS::write_ACSE_chunk(std::ostream * out, std::ostringstream * chunkout, char const * chunkname)
{
	std::string chunk(chunkout->str());

	if (chunk.size())
	{
		*out << chunkname;
		BinaryTokenACS::write_ACS0_32(out, chunk.size());
		*out << chunk;
	}

	chunkout->str("");
}

void BinaryTokenZDACS::write_ACSE_function(std::ostream * out, ObjectData_Function const & f)
{
	static ObjectExpression::Pointer const externalAddr(ObjectExpression::create_value_int(0, SourcePosition::none));

	BinaryTokenACS::write_ACS0_8 (out, f.argCount);
	BinaryTokenACS::write_ACS0_8 (out, f.varCount);
	BinaryTokenACS::write_ACS0_8 (out, !!f.retCount);
	BinaryTokenACS::write_ACS0_8 (out, 0);
	BinaryTokenACS::write_ACS0_32(out, f.external ? *externalAddr : *ObjectExpression::get_symbol(f.label, SourcePosition::none));
}

void BinaryTokenZDACS::write_ACSE_function_name(std::ostream * out, ObjectData_Function const & f)
{
	*out << f.name << '\0';
}

void BinaryTokenZDACS::write_ACSE_function_name_count(std::ostream * out, ObjectData_Function const & f)
{
	_string_offset += 4;
}

void BinaryTokenZDACS::write_ACSE_function_name_offset(std::ostream * out, ObjectData_Function const & f)
{
	BinaryTokenACS::write_ACS0_32(out, _string_offset);
	_string_offset += f.name.size() + 1;
}

void BinaryTokenZDACS::write_ACSE_library(std::ostream * out, std::string const & lib)
{
	*out << lib << '\0';
}

void BinaryTokenZDACS::write_ACSE_registerarray(std::ostream * out, ObjectData_RegisterArray const & r)
{
	BinaryTokenACS::write_ACS0_32(out, r.number);
	BinaryTokenACS::write_ACS0_32(out, r.size);
}

void BinaryTokenZDACS::write_ACSE_script(std::ostream * out, ObjectData_Script const & s)
{
	BinaryTokenACS::write_ACS0_16(out, s.number);
	BinaryTokenACS::write_ACS0_16(out, s.stype);
	BinaryTokenACS::write_ACS0_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
	BinaryTokenACS::write_ACS0_32(out, s.argCount <= 3 ? s.argCount : 3);
}

void BinaryTokenZDACS::write_ACSE_script_flags(std::ostream * out, ObjectData_Script const & s)
{
	if (!s.flags) return;

	BinaryTokenACS::write_ACS0_16(out, s.number);
	BinaryTokenACS::write_ACS0_16(out, s.flags);
}

void BinaryTokenZDACS::write_ACSE_script_vars(std::ostream * out, ObjectData_Script const & s)
{
	if (s.varCount <= 10) return;
	if (s.varCount <= 20 && target_type == TARGET_ZDoom) return;

	BinaryTokenACS::write_ACS0_16(out, s.number);
	BinaryTokenACS::write_ACS0_16(out, s.varCount);
}

void BinaryTokenZDACS::write_ACSE_string(std::ostream * out, ObjectData_String const & s)
{
	BinaryTokenACS::write_ACS0_string(out, s.string);
}
void BinaryTokenZDACS::write_ACSE_string_offset(std::ostream * out, ObjectData_String const & s)
{
	BinaryTokenACS::write_ACS0_32(out, 12 + (ObjectExpression::get_string_count() * 4) + s.offset);
}


