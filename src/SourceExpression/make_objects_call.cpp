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

/* SourceExpression/make_objects_call.cpp
**
** Defines the SourceExpression::make_objects_call function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call(ObjectVector * objects, SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position)
{
	VariableType const * type(expr->getType());

	switch (type->vt)
	{
	case VariableType::VT_ASMFUNC:
		if (expr->canMakeObject())
			make_objects_call_asmfunc(objects, type, expr->makeObject(), args, position);
		else
			throw SourceException("non-constant asmfuncs not supported", position, "SourceVariable");
		break;

	case VariableType::VT_FUNCTION:
		if (expr->canMakeObject())
			make_objects_call_function(objects, type, expr->makeObject(), args, stack, position);
		else
			throw SourceException("non-constant functions not yet supported", position, "SourceVariable");
		break;

	case VariableType::VT_LINESPEC:
		if (expr->canMakeObject())
			make_objects_call_linespec(objects, type, expr->makeObject(), args, position);
		else
			throw SourceException("non-constant linespecs not yet supported", position, "SourceVariable");
		break;

	case VariableType::VT_NATIVE:
		if (expr->canMakeObject())
			make_objects_call_native(objects, type, expr->makeObject(), args, position);
		else
			throw SourceException("non-constant natives not yet supported", position, "SourceVariable");
		break;

	case VariableType::VT_SCRIPT:
		make_objects_call_script(objects, type, expr, args, stack, position);
		break;

	default:
		throw SourceException("attempt to call uncallable", position, "SourceExpression");
	}
}



