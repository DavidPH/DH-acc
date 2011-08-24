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

/* SourceExpression.cpp
**
** Defines the SourceExpression methods.
*/

#include "SourceExpression.hpp"

#include "ObjectVector.hpp"
#include "print_debug.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "VariableType.hpp"



SourceExpression::SourceExpression(SourcePosition const & position) : evaluations(0), position(position)
{

}

void SourceExpression::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

bool SourceExpression::canMakeObject() const
{
	return false;
}
bool SourceExpression::canMakeObjectAddress() const
{
	return false;
}

bool SourceExpression::canMakeObjectsAddress() const
{
	return false;
}

VariableType const * SourceExpression::get_promoted_type(VariableType const * type1, VariableType const * type2, SourcePosition const & position)
{
	if (type1 == type2) return type1;

	if (type1->vt == VariableType::VT_VOID) return type1;
	if (type2->vt == VariableType::VT_VOID) return type2;

	if (type1->vt == VariableType::VT_POINTER) return type1;
	if (type2->vt == VariableType::VT_POINTER) return type2;

	if (type1->vt == VariableType::VT_REAL) return type1;
	if (type2->vt == VariableType::VT_REAL) return type2;

	if (type1->vt == VariableType::VT_INT) return type1;
	if (type2->vt == VariableType::VT_INT) return type2;

	if (type1->vt == VariableType::VT_CHAR) return type1;
	if (type2->vt == VariableType::VT_CHAR) return type2;

	if (type1->vt == VariableType::VT_STRING) return type1;
	if (type2->vt == VariableType::VT_STRING) return type2;

	if (type1->vt == VariableType::VT_BOOLHARD) return type1;
	if (type2->vt == VariableType::VT_BOOLHARD) return type2;

	if (type1->vt == VariableType::VT_BOOLSOFT) return type1;
	if (type2->vt == VariableType::VT_BOOLSOFT) return type2;

	return SourceContext::global_context.getVariableType(VariableType::VT_VOID);
}

VariableType const * SourceExpression::getType() const
{
	return SourceContext::global_context.getVariableType(VariableType::VT_VOID);
}

CounterPointer<ObjectExpression> SourceExpression::makeObject() const
{
	throw SourceException("makeObject on invalid expression", position, getName());
}
CounterPointer<ObjectExpression> SourceExpression::makeObjectAddress() const
{
	throw SourceException("makeObjectAddress on invalid expression", position, getName());
}

void SourceExpression::makeObjects(ObjectVector * objects)
{
	makeObjectsGet(objects);
	make_objects_cast(objects, getType(), SourceContext::global_context.getVariableType(VariableType::VT_VOID), position);
}

void SourceExpression::makeObjectsAddress(ObjectVector * objects)
{
	throw SourceException("makeObjectsAddress on invalid expression", position, getName());
}

void SourceExpression::makeObjectsCast(ObjectVector * objects, VariableType const * type)
{
	if (type->vt == VariableType::VT_VOID)
	{
		// If casting to void, use makeObjects so as to take advantage
		// of any related optimization.
		makeObjects(objects);
	}
	else
	{
		makeObjectsGet(objects);
		make_objects_cast(objects, getType(), type, position);
	}
}

void SourceExpression::makeObjectsGet(ObjectVector * objects)
{
	throw SourceException("makeObjectsGet on invalid expression", position, getName());
}
void SourceExpression::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	throw SourceException("makeObjectsGetArray on invalid expression", position, getName());
}
void SourceExpression::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	throw SourceException("makeObjectsGetMember on invalid expression", position, getName());
}

void SourceExpression::makeObjectsSet(ObjectVector * objects)
{
	throw SourceException("makeObjectsSet on invalid expression", position, getName());
}
void SourceExpression::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	throw SourceException("makeObjectsSetArray on invalid expression", position, getName());
}
void SourceExpression::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	throw SourceException("makeObjectsSetMember on invalid expression", position, getName());
}

void SourceExpression::printDebug(std::ostream * out) const
{
	*out << "SourceExpressionDS(";
		*out << "evaluations=(";
		print_debug(out, evaluations);
		*out << ")";

		*out << ", ";

		*out << "labels=(";
		print_debug(out, _labels);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, position);
		*out << ")";
	*out << ")";
}

void SourceExpression::recurse_makeObjects(ObjectVector * objects)
{
	recurse_makeObjectsGet(objects);
}

void SourceExpression::recurse_makeObjectsAddress(ObjectVector * objects)
{
	recurse_makeObjectsGet(objects);
}

void SourceExpression::recurse_makeObjectsCast(ObjectVector * objects, VariableType const * type)
{
	recurse_makeObjectsGet(objects);
}

void SourceExpression::recurse_makeObjectsGet(ObjectVector * objects)
{
	if (!evaluations++)
		objects->addLabel(_labels);

	objects->setPosition(position);
}
void SourceExpression::recurse_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	recurse_makeObjectsGet(objects);
}
void SourceExpression::recurse_makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	recurse_makeObjectsGet(objects);
}

void SourceExpression::recurse_makeObjectsSet(ObjectVector * objects)
{
	recurse_makeObjectsGet(objects);
}
void SourceExpression::recurse_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	recurse_makeObjectsGet(objects);
}
void SourceExpression::recurse_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	recurse_makeObjectsGet(objects);
}



void print_debug(std::ostream * out, SourceExpression const & in)
{
	in.printDebug(out);
}



