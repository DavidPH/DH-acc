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

/* SourceVariable/print_debug.cpp
**
** Defines the SourceVariable print_debug functions.
*/

#include "../SourceVariable.hpp"

#include "../print_debug.hpp"



void print_debug(std::ostream * const out, SourceVariable const & in)
{
	*out << "SourceVariable(";
		*out << "address=(";
		*out << in._address;
		*out << ")";

		*out << ", ";

		*out << "nameObject=(";
		print_debug(out, in._nameObject);
		*out << ")";

		*out << ", ";

		*out << "nameSource=(";
		print_debug(out, in._nameSource);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, in._position);
		*out << ")";

		*out << ", ";

		*out << "sc=(";
		print_debug(out, in._sc);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, in._type);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceVariable::StorageClass const in)
{
	switch (in)
	{
	case SourceVariable::SC_CONSTANT:             *out << "SC_CONSTANT";             break;
	case SourceVariable::SC_REGISTER:             *out << "SC_REGISTER";             break;
	case SourceVariable::SC_REGISTER_GLOBAL:      *out << "SC_REGISTER_GLOBAL";      break;
	case SourceVariable::SC_REGISTER_MAP:         *out << "SC_REGISTER_MAP";         break;
	case SourceVariable::SC_REGISTER_WORLD:       *out << "SC_REGISTER_WORLD";       break;
	case SourceVariable::SC_REGISTERARRAY_GLOBAL: *out << "SC_REGISTERARRAY_GLOBAL"; break;
	case SourceVariable::SC_REGISTERARRAY_MAP:    *out << "SC_REGISTERARRAY_MAP";    break;
	case SourceVariable::SC_REGISTERARRAY_WORLD:  *out << "SC_REGISTERARRAY_WORLD";  break;
	}
}
void print_debug(std::ostream * const out, SourceVariable::VariableType const & in)
{
	*out << "SourceVariable::VariableType(";
		*out << "type=(";
		print_debug(out, in.type);
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
void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in)
{
	switch (in)
	{
	case SourceVariable::VT_ACSFUNC: *out << "VT_ACSFUNC"; break;
	case SourceVariable::VT_ARRAY:   *out << "VT_ARRAY";   break;
	case SourceVariable::VT_ASMFUNC: *out << "VT_ASMFUNC"; break;
	case SourceVariable::VT_CHAR:    *out << "VT_CHAR";    break;
	case SourceVariable::VT_INT:     *out << "VT_INT";     break;
	case SourceVariable::VT_LNSPEC:  *out << "VT_LNSPEC";  break;
	case SourceVariable::VT_NATIVE:  *out << "VT_NATIVE";  break;
	case SourceVariable::VT_REAL:    *out << "VT_REAL" ;   break;
	case SourceVariable::VT_SCRIPT:  *out << "VT_SCRIPT";  break;
	case SourceVariable::VT_STRING:  *out << "VT_STRING";  break;
	case SourceVariable::VT_STRUCT:  *out << "VT_STRUCT";  break;
	case SourceVariable::VT_VOID:    *out << "VT_VOID";    break;
	}
}



