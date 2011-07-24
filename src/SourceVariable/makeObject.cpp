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

/* SourceVariable/makeObject.cpp
**
** Defines the SourceVariable::makeObject method.
*/

#include "../SourceVariable.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"



ObjectExpression SourceVariable::makeObject(SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_CONSTANT:
		switch (_type->type)
		{
		case VT_ACSFUNC:
			return ObjectExpression::create_value_int(_data.vdACSFunc.number, position);

		case VT_ARRAY:
		case VT_STRUCT:
			throw SourceException("makeObject on compound VT", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			throw SourceException("makeObject on void VT", position, "SourceVariable");

		case VT_CHAR:
			return ObjectExpression::create_value_int(_data.vdChar.value, position);

		case VT_INT:
			return ObjectExpression::create_value_int(_data.vdInt.value, position);

		case VT_LNSPEC:
			return ObjectExpression::create_value_int(_data.vdLnSpec.number, position);

		case VT_NATIVE:
			return ObjectExpression::create_value_int(_data.vdNative.number, position);

		case VT_REAL:
			return ObjectExpression::create_value_float(_data.vdReal.value, position);

		case VT_STRING:
			return ObjectExpression::create_value_symbol(_nameObject, position);

		case VT_SCRIPT:
			return ObjectExpression::create_value_int(_data.vdScript.number, position);
		}
		break;

	case SC_REGISTER:
		throw SourceException("makeObject on SC_REGISTER", position, "SourceVariable");

	case SC_REGISTER_GLOBAL:
		throw SourceException("makeObject on SC_REGISTER_GLOBAL", position, "SourceVariable");

	case SC_REGISTER_MAP:
		throw SourceException("makeObject on SC_REGISTER_MAP", position, "SourceVariable");

	case SC_REGISTER_WORLD:
		throw SourceException("makeObject on SC_REGISTER_WORLD", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
		throw SourceException("makeObject on SC_REGISTERARRAY_GLOBAL", position, "SourceVariable");

	case SC_REGISTERARRAY_MAP:
		throw SourceException("makeObject on SC_REGISTERARRAY_MAP", position, "SourceVariable");

	case SC_REGISTERARRAY_WORLD:
		throw SourceException("makeObject on SC_REGISTERARRAY_WORLD", position, "SourceVariable");
	}

	throw SourceException("makeObject", _position, "SourceVariable");
}



