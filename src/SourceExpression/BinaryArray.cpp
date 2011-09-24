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

/* SourceExpression/BinaryArray.cpp
**
** Defines the SourceExpression_BinaryArray class and methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryArray : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray, SourceExpression_Binary);

public:
	SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual bool canMakeObject() const;

	virtual VariableType const * getType() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjectsAccess(ObjectVector * objects);
	virtual void virtual_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

	virtual void virtual_makeObjectsGet(ObjectVector * objects);
	virtual void virtual_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

	virtual void virtual_makeObjectsSet(ObjectVector * objects);
	virtual void virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
};



SourceExpression::Pointer SourceExpression::create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	// This allows C semantics for array access. Specifically that x[y] be
	// the same as *(x+y).
	if (
		exprL->getType()->vt == VariableType::VT_POINTER ||
		exprR->getType()->vt == VariableType::VT_POINTER ||
		(exprL->getType()->vt == VariableType::VT_ARRAY && exprL->canMakeObjectsAddress()) ||
		(exprR->getType()->vt == VariableType::VT_ARRAY && exprR->canMakeObjectsAddress())
	)
	{
		return create_unary_dereference(create_binary_add(exprL, exprR, position), position);
	}

	return new SourceExpression_BinaryArray(exprL, exprR, position);
}



SourceExpression_BinaryArray::SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, NULL, VariableType::get_vt_int(), position)
{
	if (exprL->getType()->vt != VariableType::VT_ARRAY && exprL->getType()->vt != VariableType::VT_STRING)
		throw SourceException("expected VT_ARRAY or VT_STRING for exprL got " + (std::string)make_string(exprL->getType()->vt), position, getName());

	if (getType()->size() != 1)
		this->exprR = create_binary_mul(exprR, create_value_int(getType()->size(), position), position);
}

bool SourceExpression_BinaryArray::canMakeObject() const
{
	return false;
}

VariableType const * SourceExpression_BinaryArray::getType() const
{
	return exprL->getType()->refType;
}

void SourceExpression_BinaryArray::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryArray(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryArray::virtual_makeObjectsAccess(ObjectVector * objects)
{
	Super::recurse_makeObjectsAccess(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		throw SourceException("makeObjectsAccess on VT_STRING element", position, getName());
	}
	else
	{
		std::vector<SourceExpression::Pointer> dimensions(1, exprR);
		exprL->makeObjectsAccessArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::virtual_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsAccessArray(objects, dimensions);

	dimensions->push_back(exprR);
	exprL->makeObjectsAccessArray(objects, dimensions);
}

void SourceExpression_BinaryArray::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		exprL->makeObjectsGet(objects);
		exprR->makeObjectsGet(objects);

		objects->setPosition(position);

		objects->addToken(OCODE_CALLZDFUNC, objects->getValue(2), objects->getValue(15));
	}
	else
	{
		std::vector<SourceExpression::Pointer> dimensions(1, exprR);
		exprL->makeObjectsGetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::virtual_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsGetArray(objects, dimensions);

	dimensions->push_back(exprR);
	exprL->makeObjectsGetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::virtual_makeObjectsSet(ObjectVector * objects)
{
	Super::recurse_makeObjectsSet(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		throw SourceException("makeObjectsSet on VT_STRING element", position, getName());
	}
	else
	{
		std::vector<SourceExpression::Pointer> dimensions(1, exprR);
		exprL->makeObjectsSetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsSetArray(objects, dimensions);

	dimensions->push_back(exprR);
	exprL->makeObjectsSetArray(objects, dimensions);
}


