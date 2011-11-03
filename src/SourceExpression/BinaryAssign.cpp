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

/* SourceExpression/BinaryAssign.cpp
**
** Defines the SourceExpression_BinaryAssign class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryAssign : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryAssign, SourceExpression_Binary);

public:
	SourceExpression_BinaryAssign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual bool canMakeObject() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector * objects);

	virtual void virtual_makeObjectsGet(ObjectVector * objects);
};



SourceExpression::Pointer SourceExpression::create_binary_assign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryAssign(exprL, exprR, position);
}



SourceExpression_BinaryAssign::SourceExpression_BinaryAssign(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, NULL, exprL_->getType(), position_)
{

}

bool SourceExpression_BinaryAssign::canMakeObject() const
{
	return false;
}

void SourceExpression_BinaryAssign::printDebug(std::ostream * out) const
{
	*out << "SourceExpressionDS_BinaryAssign(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryAssign::virtual_makeObjects(ObjectVector * objects)
{
	Super::recurse_makeObjects(objects);

	if (evaluations == 1)
	{
		if (!exprL->getType()->constType)
			throw SourceException("attempt to assign const type", position, getName());

		exprR->makeObjectsGet(objects);
		exprL->makeObjectsSet(objects);
	}
}

void SourceExpression_BinaryAssign::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	if (evaluations == 1)
	{
		if (!exprL->getType()->constType)
			throw SourceException("attempt to assign const type", position, getName());

		exprR->makeObjectsGet(objects);
		exprL->makeObjectsAccess(objects);
	}
	else
	{
		exprL->makeObjectsGet(objects);
	}
}


