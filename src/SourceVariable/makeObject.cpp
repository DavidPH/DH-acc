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
** Defines the SourceVariable::makeObject and makeObjectAddress methods.
*/

#include "../SourceVariable.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



ObjectExpression::Pointer SourceVariable::makeObject(SourcePosition const & position) const
{
	if (_expr) return _expr;

	switch (_sc)
	{
	case SC_AUTO:
		throw SourceException("makeObject on SC_AUTO", position, "SourceVariable");

	case SC_CONSTANT:
		switch (_type->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
		case VariableType::VT_UNION:
			throw SourceException("makeObject on compound VT", position, "SourceVariable");

		case VariableType::VT_FUNCTION:
		case VariableType::VT_STRING:
		case VariableType::VT_SCRIPT:
			return ObjectExpression::create_value_symbol(_nameObject, position);

		case VariableType::VT_ASMFUNC:
		case VariableType::VT_VOID:
			throw SourceException("makeObject on void VT", position, "SourceVariable");

		case VariableType::VT_BOOLHARD:
		case VariableType::VT_BOOLSOFT:
			return ObjectExpression::create_value_int(_data.vdBool.value, position);

		case VariableType::VT_CHAR:
			return ObjectExpression::create_value_int(_data.vdChar.value, position);

		case VariableType::VT_ENUM:
			throw SourceException("makeObject on VT_ENUM", position, "SourceVariable");

		case VariableType::VT_INT:
			return ObjectExpression::create_value_int(_data.vdInt.value, position);

		case VariableType::VT_LINESPEC:
			return ObjectExpression::create_value_int(_data.vdLineSpec.number, position);

		case VariableType::VT_NATIVE:
			return ObjectExpression::create_value_int(_data.vdNative.number, position);

		case VariableType::VT_POINTER:
			throw SourceException("makeObject on VT_POINTER", position, "SourceVariable");

		case VariableType::VT_REAL:
			return ObjectExpression::create_value_float(_data.vdReal.value, position);
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

	case SC_STATIC:
		throw SourceException("makeObject on SC_STATIC", position, "SourceVariable");
	}

	throw SourceException("makeObject", _position, "SourceVariable");
}
ObjectExpression::Pointer SourceVariable::makeObjectAddress(SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_AUTO:
		throw SourceException("makeObjectAddress on SC_AUTO", position, "SourceVariable");

	case SC_CONSTANT:
		throw SourceException("makeObjectAddress on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		throw SourceException("makeObjectAddress on register", position, "SourceVariable");

	case SC_STATIC:
		return ObjectExpression::create_binary_add(ObjectExpression::create_value_symbol(_nameObject, position), ObjectExpression::static_offset, position);
	}

	throw SourceException("makeObjectAddress", _position, "SourceVariable");
}



