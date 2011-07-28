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



class SourceExpression_BinaryAssign : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryAssign, SourceExpression_Binary);

public:
	SourceExpression_BinaryAssign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_assign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryAssign(exprL, exprR, position);
}



SourceExpression_BinaryAssign::SourceExpression_BinaryAssign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, false, position)
{

}

void SourceExpression_BinaryAssign::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);
	exprR->makeObjectsGet(objects);
	exprL->makeObjectsSet(objects);
}

void SourceExpression_BinaryAssign::printDebug(std::ostream * out) const
{
	*out << "SourceExpressionDS_BinaryAssign(";
	Super::printDebug(out);
	*out << ")";
}



