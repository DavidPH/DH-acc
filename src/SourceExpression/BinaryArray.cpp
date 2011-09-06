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
#include "../VariableType.hpp"



class SourceExpression_BinaryArray : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray, SourceExpression_Binary);

public:
	SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjectsAccess(ObjectVector * objects);
	virtual void virtual_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

	virtual void virtual_makeObjectsAddress(ObjectVector * objects);

	virtual void virtual_makeObjectsGet(ObjectVector * objects);
	virtual void virtual_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);

	virtual void virtual_makeObjectsSet(ObjectVector * objects);
	virtual void virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
};



SourceExpression::Pointer SourceExpression::create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryArray(exprL, exprR, position);
}



SourceExpression_BinaryArray::SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, NULL, VariableType::get_vt_int(), position)
{

}

bool SourceExpression_BinaryArray::canMakeObject() const
{
	return false;
}
bool SourceExpression_BinaryArray::canMakeObjectAddress() const
{
	return exprL->canMakeObjectAddress() && exprR->canMakeObject();
}

bool SourceExpression_BinaryArray::canMakeObjectsAddress() const
{
	return exprL->getType()->vt == VariableType::VT_POINTER || exprL->canMakeObjectsAddress();
}

VariableType const * SourceExpression_BinaryArray::getType() const
{
	return exprL->getType()->refType;
}

CounterPointer<ObjectExpression> SourceExpression_BinaryArray::makeObjectAddress() const
{
	ObjectExpression::Pointer objSize(ObjectExpression::create_value_int(getType()->size(), position));
	ObjectExpression::Pointer objOffset(ObjectExpression::create_binary_mul(exprR->makeObject(), objSize, position));
	return ObjectExpression::create_binary_add(exprL->makeObjectAddress(), objOffset, position);
}

void SourceExpression_BinaryArray::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryArray(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryArray::virtual_makeObjectsAccess(ObjectVector * objects)
{
	if (getType()->vt == VariableType::VT_VOID)
		Super::makeObjectsAccess(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		Super::makeObjectsAccess(objects);
	}
	else if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);

		objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(getType()->size()); i--;)
			objects->addToken(OCODE_ASSIGNPOINTER, objects->getValue(i));

		for (int i(0); i < getType()->size(); ++i)
			objects->addToken(OCODE_PUSHPOINTER, objects->getValue(i));
	}
	else
	{
		Super::recurse_makeObjectsAccess(objects);

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

void SourceExpression_BinaryArray::virtual_makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsAddress(objects);

	if (exprL->getType()->vt == VariableType::VT_POINTER)
	{
		exprL->makeObjectsGet(objects);
	}
	else
	{
		exprL->makeObjectsAddress(objects);
	}

	exprR->makeObjectsGet(objects);

	objects->setPosition(position);

	if (getType()->size() != 1)
	{
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue(getType()->size()));
		objects->addToken(OCODE_MUL);
	}
	objects->addToken(OCODE_ADD);
}

void SourceExpression_BinaryArray::virtual_makeObjectsGet(ObjectVector * objects)
{
	if (getType()->vt == VariableType::VT_VOID)
		Super::makeObjectsGet(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		Super::recurse_makeObjectsGet(objects);

		exprL->makeObjectsGet(objects);
		exprR->makeObjectsGet(objects);

		objects->setPosition(position);

		objects->addToken(OCODE_CALLZDFUNC, objects->getValue(2), objects->getValue(15));
	}
	else if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);

		if (getType()->size() == 1)
		{
			objects->addToken(OCODE_PUSHGLOBALARRAY, objects->getValue(0));
		}
		else
		{
			objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

			for (int i(0); i < getType()->size(); ++i)
				objects->addToken(OCODE_PUSHPOINTER, objects->getValue(i));
		}
	}
	else
	{
		Super::recurse_makeObjectsGet(objects);

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
	if (getType()->vt == VariableType::VT_VOID)
		Super::makeObjectsSet(objects);

	if (exprL->getType()->vt == VariableType::VT_STRING)
	{
		Super::makeObjectsSet(objects);
	}
	else if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);

		if (getType()->size() == 1)
		{
			objects->addToken(OCODE_SWAP);
			objects->addToken(OCODE_ASSIGNGLOBALARRAY, objects->getValue(0));
		}
		else
		{
			objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

			for (int i(getType()->size()); i--;)
				objects->addToken(OCODE_ASSIGNPOINTER, objects->getValue(i));
		}
	}
	else
	{
		Super::recurse_makeObjectsSet(objects);

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

