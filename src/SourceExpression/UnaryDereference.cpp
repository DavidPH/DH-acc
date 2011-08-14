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

/* SourceExpression/UnaryDereference.cpp
**
** Defines the SourceExpression_UnaryDereference class and methods.
*/

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"



class SourceExpression_UnaryDereference : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryDereference, SourceExpression_Unary);

public:
	SourceExpression_UnaryDereference(SourceExpression * expr, SourcePosition const & position);

	virtual bool canMakeObjectsAddress() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsAddress(ObjectVector * objects);

	virtual void makeObjectsGet(ObjectVector * objects);

	virtual void makeObjectsSet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;
};



SourceExpression::Pointer SourceExpression::create_unary_dereference(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDereference(expr, position);
}



SourceExpression_UnaryDereference::SourceExpression_UnaryDereference(SourceExpression * expr, SourcePosition const & position) : Super(expr, NULL, position)
{

}

bool SourceExpression_UnaryDereference::canMakeObjectsAddress() const
{
	return true;
}

SourceVariable::VariableType const * SourceExpression_UnaryDereference::getType() const
{
	return expr->getType()->refType;
}

void SourceExpression_UnaryDereference::makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);
}

void SourceExpression_UnaryDereference::makeObjectsGet(ObjectVector * objects)
{
	if (getType()->type == SourceVariable::VT_VOID)
		Super::makeObjectsGet(objects);

	if (expr->getType()->type == SourceVariable::VT_STRING)
	{
		Super::recurse_makeObjectsGet(objects);
		objects->addTokenPushZero();
		objects->addToken(OCODE_CALLZDFUNC, objects->getValue(2), objects->getValue(15));
	}
	else
	{
		makeObjectsAddress(objects);
		objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(0); i < getType()->size(); ++i)
		{
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(OCODE_ADD);
			objects->addToken(OCODE_PUSHGLOBALARRAY, objects->getValue(0));
		}
	}
}

void SourceExpression_UnaryDereference::makeObjectsSet(ObjectVector * objects)
{
	if (getType()->type == SourceVariable::VT_VOID)
		Super::makeObjectsSet(objects);

	if (expr->getType()->type == SourceVariable::VT_STRING)
	{
		Super::makeObjectsSet(objects);
	}
	else
	{
		makeObjectsAddress(objects);
		objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(getType()->size()); i--;)
		{
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(OCODE_ADD);
			objects->addToken(OCODE_SWAP);
			objects->addToken(OCODE_ASSIGNGLOBALARRAY, objects->getValue(0));
		}

		for (int i(0); i < getType()->size(); ++i)
		{
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(i));
			objects->addToken(OCODE_PUSHWORLDVAR, objects->getValue(1));
			objects->addToken(OCODE_ADD);
			objects->addToken(OCODE_PUSHGLOBALARRAY, objects->getValue(0));
		}
	}
}

void SourceExpression_UnaryDereference::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_UnaryDereference(";
	Super::printDebug(out);
	*out << ")";
}



