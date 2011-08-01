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

/* SourceVariable/VariableType.cpp
**
** Defines the SourceVariable::VariableType methods.
*/

#include "../SourceVariable.hpp"

#include "../SourceException.hpp"



int SourceVariable::VariableType::getOffset(std::string const & name, SourcePosition const & position) const
{
	int offset(0);

	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return offset;
		else
			offset += types[i]->size();

	throw SourceException("invalid member-variable-type", position, "SourceVariable::VariableType");
}

SourceVariable::VariableType const * SourceVariable::VariableType::getType(std::string const & name, SourcePosition const & position) const
{
	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return types[i];

	throw SourceException("invalid member-variable-type", position, "SourceVariable::VariableType");
}

bool SourceVariable::VariableType::isVoid() const
{
	switch (type)
	{
	case VT_ACSFUNC:
	case VT_ASMFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return false;

	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
	{
		for (size_t i(0); i < types.size(); ++i)
			if (!types[i]->isVoid()) return false;

		return true;
	}

	case VT_VOID:
		return true;
	}

	return 0;
}

int SourceVariable::VariableType::size() const
{
	switch (type)
	{
	case VT_ACSFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return 1;

	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
	{
		int s(0);

		for (size_t i(0); i < types.size(); ++i)
			s += types[i]->size();

		return s;
	}

	case VT_ASMFUNC:
	case VT_VOID:
		return 0;
	}

	return 0;
}



