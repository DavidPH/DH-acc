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

/* SourceVariable/makeObjectsCall.cpp
**
** Defines the SourceVariable::makeObjectsCall method.
*/

#include "../SourceVariable.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../SourceExpressionDS.hpp"



void SourceVariable::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position) const
{
	objects->setPosition(position);

	switch (_type->type)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_INT:
	case VT_POINTER:
	case VT_REAL:
	case VT_STRING:
	case VT_STRUCT:
	case VT_VOID:
		throw SourceException("attempt to call uncallable", position, "SourceVariable");

	case VT_ASMFUNC:
		if (_sc == SC_CONSTANT)
			SourceExpression::make_objects_call_asmfunc(objects, _data.vdAsmFunc, args, position);
		else
			throw SourceException("non-constant asmfuncs not supported", position, "SourceVariable");
		break;

	case VT_FUNCTION:
		if (_sc == SC_CONSTANT)
			SourceExpression::make_objects_call_function(objects, _data.vdFunction, args, stack, position);
		else
			throw SourceException("non-constant functions not yet supported", position, "SourceVariable");
		break;

	case VT_LINESPEC:
		if (_sc == SC_CONSTANT)
			SourceExpression::make_objects_call_linespec(objects, _data.vdLineSpec, args, position);
		else
			throw SourceException("non-constant linespecs not yet supported", position, "SourceVariable");
		break;

	case VT_NATIVE:
		if (_sc == SC_CONSTANT)
			SourceExpression::make_objects_call_native(objects, _data.vdNative, args, position);
		else
			throw SourceException("non-constant natives not yet supported", position, "SourceVariable");
		break;

	case VT_SCRIPT:
		makeObjectsGet(objects, position);
		SourceExpression::make_objects_call_script(objects, _type, args, stack, position);
		break;
	}
}



