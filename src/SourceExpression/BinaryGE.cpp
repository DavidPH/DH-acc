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

/* SourceExpression/BinaryGE.cpp
**
** Defines the SourceExpression_BinaryGE class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryGE : public SourceExpression_BinaryCompare
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryGE, SourceExpression_BinaryCompare);

public:
	SourceExpression_BinaryGE(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);
};



SourceExpression::Pointer SourceExpression::create_binary_ge(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryGE(exprL, exprR, position);
}



SourceExpression_BinaryGE::SourceExpression_BinaryGE(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, position_)
{

}

void SourceExpression_BinaryGE::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryGE(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryGE::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (exprL->getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_FUNCTION:
	case VariableType::VT_INT:
	case VariableType::VT_LINESPEC:
	case VariableType::VT_NATIVE:
	case VariableType::VT_POINTER:
	case VariableType::VT_REAL:
	case VariableType::VT_SCRIPT:
	case VariableType::VT_STRING:
		objects->addToken(OCODE_CMPGE);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}


