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

/* SourceExpression/BinaryShift.cpp
**
** Defines the SourceExpression_BinaryShift class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"



class SourceExpression_BinaryShift : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryShift, SourceExpression_Binary);

public:
	SourceExpression_BinaryShift(SourceExpression * exprL, SourceExpression * exprR, bool right, SourcePosition const & position);

protected:
	virtual void makeObjectsGet(ObjectVector * objects);

	virtual void printDebug(std::ostream * const out) const;

private:
	bool _right;
};



SourceExpression::Pointer SourceExpression::create_binary_shiftl(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryShift(exprL, exprR, false, position);
}
SourceExpression::Pointer SourceExpression::create_binary_shiftr(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryShift(exprL, exprR, true, position);
}



SourceExpression_BinaryShift::SourceExpression_BinaryShift(SourceExpression * exprL, SourceExpression * exprR, bool right, SourcePosition const & position) : Super(exprL, exprR, SourceVariable::get_VariableType(SourceVariable::VT_INT), SourceVariable::get_VariableType(SourceVariable::VT_INT), false, position), _right(right)
{

}

void SourceExpression_BinaryShift::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	objects->addToken(_right ? OCODE_SHIFTR : OCODE_SHIFTL);
}

void SourceExpression_BinaryShift::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryShift(";
	Super::printDebug(out);
	*out << ")";
}



