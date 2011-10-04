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

/* SourceExpression/BinaryIOr.cpp
**
** Defines the SourceExpression_BinaryIOr class and methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryIOr : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryIOr, SourceExpression_Binary);

public:
	SourceExpression_BinaryIOr(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual CounterPointer<ObjectExpression> makeObject() const;

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);
};



SourceExpression::Pointer SourceExpression::create_binary_ior(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryIOr(exprL, exprR, position);
}



SourceExpression_BinaryIOr::SourceExpression_BinaryIOr(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, false, position_)
{

}

CounterPointer<ObjectExpression> SourceExpression_BinaryIOr::makeObject() const
{
	return ObjectExpression::create_binary_ior(exprL->makeObject(), exprR->makeObject(), position);
}

void SourceExpression_BinaryIOr::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryIOr(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryIOr::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
	case VariableType::VT_POINTER:
		objects->addToken(OCODE_IOR);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}


