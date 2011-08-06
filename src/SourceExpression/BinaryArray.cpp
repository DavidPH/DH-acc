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

#include "../ObjectVector.hpp"



class SourceExpression_BinaryArray : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray, SourceExpression_Binary);

public:
	SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual bool canMakeObjectsAddress() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsAddress(ObjectVector * objects);

	virtual void makeObjectsGet(ObjectVector * objects);
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

	virtual void makeObjectsSet(ObjectVector * objects);
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

protected:
	virtual void printDebug(std::ostream * out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryArray(exprL, exprR, position);
}



SourceExpression_BinaryArray::SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, NULL, SourceVariable::get_VariableType(SourceVariable::VT_INT), position)
{

}

bool SourceExpression_BinaryArray::canMakeObjectsAddress() const
{
	return exprL->getType()->type == SourceVariable::VT_POINTER || exprL->canMakeObjectsAddress();
}

SourceVariable::VariableType const * SourceExpression_BinaryArray::getType() const
{
	return exprL->getType()->refType;
}

void SourceExpression_BinaryArray::makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsAddress(objects);

	if (exprL->getType()->type == SourceVariable::VT_POINTER)
	{
		exprL->makeObjectsGet(objects);
	}
	else
	{
		exprL->makeObjectsAddress(objects);
	}

	exprR->makeObjectsGet(objects);

	objects->setPosition(position);

	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(getType()->size()));
	objects->addToken(ObjectToken::OCODE_MUL);
	objects->addToken(ObjectToken::OCODE_ADD);
}

void SourceExpression_BinaryArray::makeObjectsGet(ObjectVector * objects)
{
	if (getType()->type == SourceVariable::VT_VOID)
		Super::makeObjectsGet(objects);

	if (exprL->getType()->type == SourceVariable::VT_STRING)
	{
		Super::recurse_makeObjectsGet(objects);

		exprL->makeObjectsGet(objects);
		exprR->makeObjectsGet(objects);

		objects->setPosition(position);

		objects->addToken(ObjectToken::OCODE_CALLZDFUNC, objects->getValue(2), objects->getValue(15));
	}
	else if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);

		objects->addToken(ObjectToken::OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(0); i < getType()->size(); ++i)
		{
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(ObjectToken::OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ObjectToken::OCODE_PUSHGLOBALARRAY, objects->getValue(0));
		}
	}
	else
	{
		Super::recurse_makeObjectsGet(objects);

		std::vector<SourceExpression::Pointer> dimensions(1, exprR);
		exprL->makeObjectsGetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsGetArray(objects, dimensions);

	dimensions->push_back(exprR);
	exprL->makeObjectsGetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::makeObjectsSet(ObjectVector * objects)
{
	if (getType()->type == SourceVariable::VT_VOID)
		Super::makeObjectsSet(objects);

	if (exprL->getType()->type == SourceVariable::VT_STRING)
	{
		Super::makeObjectsSet(objects);
	}
	else if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);

		objects->addToken(ObjectToken::OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(getType()->size()); i--;)
		{
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(ObjectToken::OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ObjectToken::OCODE_SWAP);
			objects->addToken(ObjectToken::OCODE_ASSIGNGLOBALARRAY, objects->getValue(0));
		}

		for (int i(0); i < getType()->size(); ++i)
		{
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(ObjectToken::OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ObjectToken::OCODE_PUSHGLOBALARRAY, objects->getValue(0));
		}
	}
	else
	{
		Super::recurse_makeObjectsSet(objects);

		std::vector<SourceExpression::Pointer> dimensions(1, exprR);
		exprL->makeObjectsSetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsSetArray(objects, dimensions);

	dimensions->push_back(exprR);
	exprL->makeObjectsSetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryArray(";
	Super::printDebug(out);
	*out << ")";
}



