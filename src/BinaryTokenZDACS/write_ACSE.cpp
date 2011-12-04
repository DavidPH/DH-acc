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



static std::vector<std::string> _strings_temp;



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

void BinaryTokenZDACS::write_ACSE_function_FUNC(std::ostream * out, ObjectData_Function const & f)
{
	static ObjectExpression::Pointer const externalAddr(ObjectExpression::create_value_int(0, SourcePosition::none));

	BinaryTokenACS::write_ACS0_8 (out, f.argCount);
	BinaryTokenACS::write_ACS0_8 (out, f.varCount);
	BinaryTokenACS::write_ACS0_8 (out, !!f.retCount);
	BinaryTokenACS::write_ACS0_8 (out, 0);
	BinaryTokenACS::write_ACS0_32(out, f.externDef ? *externalAddr : *ObjectExpression::get_symbol(f.label, SourcePosition::none));
}

void BinaryTokenZDACS::write_ACSE_function_FNAM(std::ostream * out, ObjectData_Function const & f)
{
	_strings_temp.push_back(f.name);
}

void BinaryTokenZDACS::write_ACSE_library(std::ostream * out, std::string const & lib)
{
	*out << lib << '\0';
}

void BinaryTokenZDACS::write_ACSE_register_MIMP(std::ostream * out, ObjectData_Register const & r)
{
	if (!r.externDef) return;

	// A bit of a hack, really.
	for (int i = 0; i < r.size; i++)
	{
		BinaryTokenACS::write_ACS0_32(out, r.number+i);
		*out << r.name << (char)(i+1) << '\0';
	}
}

void BinaryTokenZDACS::write_ACSE_register_MEXP(std::ostream * out, ObjectData_Register const & r)
{
	if (!r.externVis) return;

	std::string s(r.name);
	s += '\0';

	if (_strings_temp.size() < r.number + r.size)
		_strings_temp.resize(r.number + r.size);

	// A bit of a hack, really.
	for (int i = 0; i < r.size; i++)
	{
		s[s.size()-1] = (char)(i+1);
		_strings_temp[r.number+i] = s;
	}
}

void BinaryTokenZDACS::write_ACSE_registerarray_ARAY(std::ostream * out, ObjectData_RegisterArray const & r)
{
	if (!r.externDef) return;

	BinaryTokenACS::write_ACS0_32(out, r.number);
	BinaryTokenACS::write_ACS0_32(out, r.size);
}

void BinaryTokenZDACS::write_ACSE_registerarray_AIMP(std::ostream * out, ObjectData_RegisterArray const & r)
{
	if (!r.externDef) return;

	BinaryTokenACS::write_ACS0_32(out, r.number);
	BinaryTokenACS::write_ACS0_32(out, r.size);
	*out << r.name << '\0';
}

void BinaryTokenZDACS::write_ACSE_registerarray_AIMP_counter(std::ostream * out, ObjectData_RegisterArray const & r)
{
	if (!r.externDef) return;

	++_temp_counter;
}

void BinaryTokenZDACS::write_ACSE_registerarray_MEXP(std::ostream * out, ObjectData_RegisterArray const & r)
{
	if (!r.externDef) return;

	if (_strings_temp.size() < r.number + 1)
		_strings_temp.resize(r.number + 1);

	_strings_temp[r.number] = r.name;
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

void BinaryTokenZDACS::write_ACSE_string_STRL(std::ostream * out, ObjectData_String const & s)
{
	_strings_temp.push_back(s.string);
}

void BinaryTokenZDACS::write_ACSE_stringtable(std::ostream * out, bool junk)
{
	std::vector<std::string>::const_iterator it;
	bigsint stringCount(_strings_temp.size());
	bigsint offset(12 + stringCount*4);

	// Header (if writing any strings)
	if (stringCount)
	{
		if (junk) BinaryTokenACS::write_ACS0_32(out, 0);
		BinaryTokenACS::write_ACS0_32(out, stringCount);
		if (junk) BinaryTokenACS::write_ACS0_32(out, 0);
	}

	// Offsets
	for (it = _strings_temp.begin(); it != _strings_temp.end(); ++it)
	{
		BinaryTokenACS::write_ACS0_32(out, offset);
		offset += it->size();

		// Ensure null termination.
		if (it->size() == 0 || (*it)[it->size()-1])
			++offset;
	}

	// Strings
	for (it = _strings_temp.begin(); it != _strings_temp.end(); ++it)
	{
		BinaryTokenACS::write_ACS0_string(out, *it);

		// Ensure null termination.
		if (it->size() == 0 || (*it)[it->size()-1])
			*out << '\0';
	}

	_strings_temp.clear();
}


