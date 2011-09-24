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

/* SourceVariable/makeObjectsAccess.cpp
**
** Defines the SourceVariable::makeObjectsAccess* methods.
*/

#include "../SourceVariable.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceExpression.hpp"
#include "../VariableType.hpp"



void SourceVariable::makeObjectsAccess(ObjectVector * objects, SourcePosition const & position) const
{
	makeObjectsSet(objects, position);
	makeObjectsGet(objects, position);
}

void SourceVariable::makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, SourcePosition const & position) const
{
	makeObjectsSetArray(objects, dimensions, position);
	makeObjectsGetArray(objects, dimensions, position);
}

void SourceVariable::makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	std::vector<std::string> namesOriginal(*names);
	makeObjectsSetMember(objects, names, position);
	makeObjectsGetMember(objects, &namesOriginal, position);
}

void SourceVariable::makeObjectsAccessPrep(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, ObjectExpression::Pointer * addressBase, int * address, SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_AUTO:
	case SC_STATIC:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		if (dimensions)
		{
			VariableType const * type = _type;

			if ((*dimensions)[0])
			{
				type = type->refType;
				SourceExpression::Pointer expr((*dimensions)[0]);
				(*dimensions)[0] = NULL;

				for (size_t i(1); i < dimensions->size(); ++i)
				{
					type = type->refType;
					expr = SourceExpression::create_binary_add(expr, (*dimensions)[i], position);
					(*dimensions)[i] = NULL;
				}

				expr->makeObjectsGet(objects);
				objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));
			}
			else
			{
				// We've already calculated the index, don't
				// need to do anything but reset address.
				for (size_t i(dimensions->size()); i--;)
					type = type->refType;
			}

			*addressBase = objects->getValue(0);
			*address = type->size(position) - 1;
		}
		else if (_sc == SC_AUTO)
		{
			*addressBase = objects->getValue(_nameObject);
			*address = _type->size(position) - 1;
		}
		else
		{
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(0));

			objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

			*addressBase = objects->getValue(0);
			*address = _type->size(position) - 1;
		}

		break;

	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*addressBase = objects->getValue(_nameObject);
		*address = _type->size(position) - 1;
		break;
	}
}


