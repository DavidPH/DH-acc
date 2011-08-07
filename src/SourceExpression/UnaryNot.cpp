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

/* SourceExpression/UnaryNot.cpp
**
** Defines the SourceExpression_UnaryNot class and methods.
*/

#include "Unary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_UnaryNot : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryNot, SourceExpression_Unary);

public:
	SourceExpression_UnaryNot(SourceExpression * expr, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;
};



SourceExpression::Pointer SourceExpression::create_unary_not(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryNot(expr, position);
}



SourceExpression_UnaryNot::SourceExpression_UnaryNot(SourceExpression * expr, SourcePosition const & position) : Super(expr, NULL, position)
{

}

void SourceExpression_UnaryNot::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (getType()->type)
	{
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_POINTER:
		objects->addToken(OCODE_NOT);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}

void SourceExpression_UnaryNot::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_UnaryNot(";
	Super::printDebug(out);
	*out << ")";
}


