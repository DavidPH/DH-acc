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

#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"
#include "VariableType.hpp"



SourceExpression::SourceExpression(SourcePosition const & position_) : evaluations(0), position(position_)
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

VariableType const * SourceExpression::get_promoted_type(VariableType const * type1, VariableType const * type2, SourcePosition const &)
{
	if (type1 == type2) return type1;

	if (type1->vt == VariableType::VT_VOID) return type1;
	if (type2->vt == VariableType::VT_VOID) return type2;

	if (type1->vt == VariableType::VT_POINTER) return type1;
	if (type2->vt == VariableType::VT_POINTER) return type2;

	if (type1->vt == VariableType::VT_ARRAY) return VariableType::get_pointer(type1->refType);
	if (type2->vt == VariableType::VT_ARRAY) return VariableType::get_pointer(type2->refType);

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

	return VariableType::get_vt_void();
}

VariableType const * SourceExpression::getType() const
{
	return VariableType::get_vt_void();
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
	makeObjectsBase(objects);

	if (_labels.empty() && canMakeObject())
	{
		recurse_makeObjects(objects);
	}
	else
		virtual_makeObjects(objects);
}

void SourceExpression::makeObjectsAccess(ObjectVector * objects)
{
	makeObjectsBase(objects);

	virtual_makeObjectsAccess(objects);
}
void SourceExpression::makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	makeObjectsBase(objects);

	virtual_makeObjectsAccessArray(objects, dimensions);
}
void SourceExpression::makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> * names)
{
	makeObjectsBase(objects);

	virtual_makeObjectsAccessMember(objects, names);
}

void SourceExpression::makeObjectsAddress(ObjectVector * objects)
{
	makeObjectsBase(objects);

	if (_labels.empty() && canMakeObjectAddress())
	{
		recurse_makeObjectsAddress(objects);

		objects->addToken(OCODE_GET_LITERAL32I, makeObjectAddress());
	}
	else
		virtual_makeObjectsAddress(objects);
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
		makeObjectsBase(objects);

		virtual_makeObjectsCast(objects, type);
	}
}

void SourceExpression::makeObjectsBase(ObjectVector * objects)
{
	if (!evaluations++)
		objects->addLabel(_labels);
}

void SourceExpression::makeObjectsGet(ObjectVector * objects)
{
	makeObjectsBase(objects);

	if (_labels.empty() && canMakeObject())
	{
		recurse_makeObjectsGet(objects);

		// FIXME: Should be based on type.
		objects->addToken(OCODE_GET_LITERAL32I, makeObject());
	}
	else
		virtual_makeObjectsGet(objects);
}
void SourceExpression::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	makeObjectsBase(objects);

	virtual_makeObjectsGetArray(objects, dimensions);
}
void SourceExpression::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	makeObjectsBase(objects);

	virtual_makeObjectsGetMember(objects, names);
}

void SourceExpression::makeObjectsSet(ObjectVector * objects)
{
	makeObjectsBase(objects);

	virtual_makeObjectsSet(objects);
}
void SourceExpression::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	makeObjectsBase(objects);

	virtual_makeObjectsSetArray(objects, dimensions);
}
void SourceExpression::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	makeObjectsBase(objects);

	virtual_makeObjectsSetMember(objects, names);
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
	objects->setPosition(position);
}

void SourceExpression::recurse_makeObjectsAccess(ObjectVector * objects)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> *)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> *)
{
	recurse_makeObjects(objects);
}

void SourceExpression::recurse_makeObjectsAddress(ObjectVector * objects)
{
	recurse_makeObjects(objects);
}

void SourceExpression::recurse_makeObjectsCast(ObjectVector * objects, VariableType const *)
{
	recurse_makeObjects(objects);
}

void SourceExpression::recurse_makeObjectsGet(ObjectVector * objects)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> *)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> *)
{
	recurse_makeObjects(objects);
}

void SourceExpression::recurse_makeObjectsSet(ObjectVector * objects)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> *)
{
	recurse_makeObjects(objects);
}
void SourceExpression::recurse_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> *)
{
	recurse_makeObjects(objects);
}

void SourceExpression::virtual_makeObjects(ObjectVector * objects)
{
	virtual_makeObjectsGet(objects);
	make_objects_cast(objects, getType(), VariableType::get_vt_void(), position);
}

void SourceExpression::virtual_makeObjectsAccess(ObjectVector *)
{
	throw SourceException("makeObjectsAccess on invalid expression", position, getName());
}
void SourceExpression::virtual_makeObjectsAccessArray(ObjectVector *, std::vector<SourceExpression::Pointer> *)
{
	throw SourceException("makeObjectsAccessArray on invalid expression", position, getName());
}
void SourceExpression::virtual_makeObjectsAccessMember(ObjectVector *, std::vector<std::string> *)
{
	throw SourceException("makeObjectsAccessMember on invalid expression", position, getName());
}

void SourceExpression::virtual_makeObjectsAddress(ObjectVector *)
{
	throw SourceException("makeObjectsAddress on invalid expression", position, getName());
}

void SourceExpression::virtual_makeObjectsCast(ObjectVector * objects, VariableType const * type)
{
	virtual_makeObjectsGet(objects);
	make_objects_cast(objects, getType(), type, position);
}

void SourceExpression::virtual_makeObjectsGet(ObjectVector *)
{
	throw SourceException("makeObjectsGet on invalid expression", position, getName());
}
void SourceExpression::virtual_makeObjectsGetArray(ObjectVector *, std::vector<SourceExpression::Pointer> *)
{
	throw SourceException("makeObjectsGetArray on invalid expression", position, getName());
}
void SourceExpression::virtual_makeObjectsGetMember(ObjectVector *, std::vector<std::string> *)
{
	throw SourceException("makeObjectsGetMember on invalid expression", position, getName());
}

void SourceExpression::virtual_makeObjectsSet(ObjectVector * objects)
{
	virtual_makeObjectsAccess(objects);
	make_objects_cast(objects, getType(), VariableType::get_vt_void(), position);
}
void SourceExpression::virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	virtual_makeObjectsAccessArray(objects, dimensions);
	make_objects_cast(objects, getType(), VariableType::get_vt_void(), position);
}
void SourceExpression::virtual_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	virtual_makeObjectsAccessMember(objects, names);
	make_objects_cast(objects, getType(), VariableType::get_vt_void(), position);
}



void print_debug(std::ostream * out, SourceExpression const & in)
{
	in.printDebug(out);
}


