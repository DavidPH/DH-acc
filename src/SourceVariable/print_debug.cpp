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
#include "../VariableType.hpp"



void print_debug(std::ostream * const out, SourceVariable const & in)
{
	*out << "SourceVariable(";
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
	case SourceVariable::SC_AUTO:                 *out << "SC_AUTO";                 break;
	case SourceVariable::SC_CONSTANT:             *out << "SC_CONSTANT";             break;
	case SourceVariable::SC_REGISTER:             *out << "SC_REGISTER";             break;
	case SourceVariable::SC_REGISTER_GLOBAL:      *out << "SC_REGISTER_GLOBAL";      break;
	case SourceVariable::SC_REGISTER_MAP:         *out << "SC_REGISTER_MAP";         break;
	case SourceVariable::SC_REGISTER_WORLD:       *out << "SC_REGISTER_WORLD";       break;
	case SourceVariable::SC_REGISTERARRAY_GLOBAL: *out << "SC_REGISTERARRAY_GLOBAL"; break;
	case SourceVariable::SC_REGISTERARRAY_MAP:    *out << "SC_REGISTERARRAY_MAP";    break;
	case SourceVariable::SC_REGISTERARRAY_WORLD:  *out << "SC_REGISTERARRAY_WORLD";  break;
	case SourceVariable::SC_STATIC:               *out << "SC_STATIC";               break;
	}
}



