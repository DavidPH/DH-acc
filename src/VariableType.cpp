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

/* VariableType.cpp
**
** Defines the VariableType methods.
*/

#include "VariableType.hpp"

#include "print_debug.hpp"
#include "SourceException.hpp"



int VariableType::getOffset(std::string const & name, SourcePosition const & position) const
{
	int offset(0);

	if (vt == VT_UNION)
		return offset;

	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return offset;
		else
			offset += types[i]->size();

	throw SourceException("invalid member-variable-type", position, "VariableType");
}

VariableType const * VariableType::getType(std::string const & name, SourcePosition const & position) const
{
	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return types[i];

	throw SourceException("invalid member-variable-type", position, "VariableType");
}

bool VariableType::isVoid() const
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
	case VT_UNION:
	{
		for (size_t i(0); i < types.size(); ++i)
			if (!types[i]->isVoid()) return false;

		return true;
	}

	case VT_VOID:
		return true;

	case VT_ASMFUNC:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_FUNCTION:
	case VT_INT:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return false;
	}

	return 0;
}

int VariableType::size() const
{
	switch (vt)
	{
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

	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_FUNCTION:
	case VT_INT:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return 1;

	case VT_UNION:
	{
		int s(0);

		for (size_t i(0); i < types.size(); ++i)
		{
			if (types[i]->size() > s)
				s = types[i]->size();
		}

		return s;
	}
	}

	return 0;
}

int VariableType::sizeCall() const
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_INT:
	case VT_POINTER:
	case VT_REAL:
	case VT_STRING:
	case VT_STRUCT:
	case VT_UNION:
	case VT_VOID:
		return 0;

	case VT_ASMFUNC:
	case VT_FUNCTION:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_SCRIPT:
	{
		int s(0);

		for (std::vector<VariableType const *>::const_iterator it(types.begin()); it != types.end(); ++it)
			s += (*it)->size();

		return s;
	}
	}

	return 0;
}



VariableType::Type & operator ++ (VariableType::Type & vt)
{
	if (vt < VariableType::VT_UNION)
		vt = (VariableType::Type)((int)vt + 1);
	else
		vt = VariableType::VT_BOOLHARD;

	return vt;
}

void print_debug(std::ostream * out, VariableType const & in)
{
	*out << "VariableType(";
		*out << "vt=(";
		print_debug(out, in.vt);
		*out << ")";

		*out << ", ";

		*out << "callType->vt=(";
		print_debug(out, in.callType->vt);
		*out << ")";

		*out << ", ";

		*out << "refType->vt=(";
		print_debug(out, in.refType->vt);
		*out << ")";

		*out << ", ";

		*out << "names=(";
		print_debug(out, in.names);
		*out << ")";

		*out << ", ";

		*out << "types=(";
		print_debug(out, in.types);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * out, VariableType::Type in)
{
	switch (in)
	{
	case VariableType::VT_ARRAY:    *out << "VT_ARRAY";    break;
	case VariableType::VT_ASMFUNC:  *out << "VT_ASMFUNC";  break;
	case VariableType::VT_BLOCK:    *out << "VT_BLOCK";    break;
	case VariableType::VT_BOOLHARD: *out << "VT_BOOLHARD"; break;
	case VariableType::VT_BOOLSOFT: *out << "VT_BOOLSOFT"; break;
	case VariableType::VT_CHAR:     *out << "VT_CHAR";     break;
	case VariableType::VT_ENUM:     *out << "VT_ENUM";     break;
	case VariableType::VT_FUNCTION: *out << "VT_FUNCTION"; break;
	case VariableType::VT_INT:      *out << "VT_INT";      break;
	case VariableType::VT_LINESPEC: *out << "VT_LINESPEC"; break;
	case VariableType::VT_NATIVE:   *out << "VT_NATIVE";   break;
	case VariableType::VT_POINTER:  *out << "VT_POINTER";  break;
	case VariableType::VT_REAL:     *out << "VT_REAL" ;    break;
	case VariableType::VT_SCRIPT:   *out << "VT_SCRIPT";   break;
	case VariableType::VT_STRING:   *out << "VT_STRING";   break;
	case VariableType::VT_STRUCT:   *out << "VT_STRUCT";   break;
	case VariableType::VT_UNION:    *out << "VT_UNION";    break;
	case VariableType::VT_VOID:     *out << "VT_VOID";     break;
	}
}



