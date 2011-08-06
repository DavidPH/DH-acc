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

/* SourceExpression/BinaryXOr.cpp
**
** Defines the SourceExpression_BinaryXOr class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_BinaryXOr : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryXOr, SourceExpression_Binary);

public:
	SourceExpression_BinaryXOr(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_xor(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryXOr(exprL, exprR, position);
}



SourceExpression_BinaryXOr::SourceExpression_BinaryXOr(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, false, position)
{

}

void SourceExpression_BinaryXOr::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (getType()->type)
	{
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_POINTER:
		objects->addToken(ObjectToken::OCODE_XOR);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}

void SourceExpression_BinaryXOr::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryXOr(";
	Super::printDebug(out);
	*out << ")";
}



