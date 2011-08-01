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

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"



class SourceExpression_BinaryArray : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray, SourceExpression);

public:
	SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual bool canMakeObjectsAddress() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsAddress(ObjectVector * objects) const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;

	virtual void printDebug(std::ostream * out) const;

protected:
	SourceExpression::Pointer _exprL;
	SourceExpression::Pointer _exprR;
};



SourceExpression::Pointer SourceExpression::create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryArray(exprL, exprR, position);
}



SourceExpression_BinaryArray::SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(position), _exprL(exprL), _exprR(exprR)
{
	SourceVariable::VariableType const * type(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	if (_exprR->getType() != type)
		_exprR = create_value_cast(_exprR, type, position);
}

bool SourceExpression_BinaryArray::canMakeObjectsAddress() const
{
	return _exprL->canMakeObjectsAddress();
}

SourceVariable::VariableType const * SourceExpression_BinaryArray::getType() const
{
	return _exprL->getType()->refType;
}

void SourceExpression_BinaryArray::makeObjectsAddress(ObjectVector * objects) const
{
	if (canMakeObjectsAddress())
	{
		objects->addLabel(labels);

		_exprL->makeObjectsAddress(objects);
		_exprR->makeObjectsGet(objects);
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(getType()->size()));
		objects->addToken(ObjectToken::OCODE_MUL);
		objects->addToken(ObjectToken::OCODE_ADD);
	}
	else
	{
		Super::makeObjectsAddress(objects);
	}
}

void SourceExpression_BinaryArray::makeObjectsGet(ObjectVector * objects) const
{
	if (getType()->type == SourceVariable::VT_VOID)
		return Super::makeObjectsGet(objects);

	if (_exprL->getType()->type == SourceVariable::VT_STRING)
	{
		objects->addLabel(labels);

		_exprL->makeObjectsGet(objects);
		_exprR->makeObjectsGet(objects);
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
		objects->addLabel(labels);

		std::vector<SourceExpression::Pointer> dimensions(1, _exprR);
		_exprL->makeObjectsGetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	objects->addLabel(labels);

	dimensions->push_back(_exprR);
	_exprL->makeObjectsGetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::makeObjectsSet(ObjectVector * objects) const
{
	if (getType()->type == SourceVariable::VT_VOID)
		return Super::makeObjectsSet(objects);

	if (_exprL->getType()->type == SourceVariable::VT_STRING)
	{
		return Super::makeObjectsSet(objects);
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
	}
	else
	{
		objects->addLabel(labels);

		std::vector<SourceExpression::Pointer> dimensions(1, _exprR);
		_exprL->makeObjectsSetArray(objects, &dimensions);
	}
}
void SourceExpression_BinaryArray::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	objects->addLabel(labels);

	dimensions->push_back(_exprR);
	_exprL->makeObjectsSetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryArray(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprL=(";
		print_debug(out, _exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, _exprR);
		*out << ")";
	*out << ")";
}



