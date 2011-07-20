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

/* ObjectExpression/BinaryAnd.cpp
**
** Defines the ObjectExpression_BinaryAnd class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryAnd : public ObjectExpression_Binary
{
public:
	ObjectExpression_BinaryAnd(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position);

	virtual ObjectExpression_BinaryAnd * clone() const;

	virtual char const * getName() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::int_t resolveInt() const;
};



ObjectExpression ObjectExpression::create_binary_and(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position)
{
	return ObjectExpression_BinaryAnd(exprL, exprR, position);
}



ObjectExpression_BinaryAnd::ObjectExpression_BinaryAnd(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position) : ObjectExpression_Binary(exprL, exprR, position)
{

}

ObjectExpression_BinaryAnd * ObjectExpression_BinaryAnd::clone() const
{
	return new ObjectExpression_BinaryAnd(*this);
}

char const * ObjectExpression_BinaryAnd::getName() const
{
	return "ObjectExpression_BinaryAnd";
}

void ObjectExpression_BinaryAnd::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_BinaryAnd(";
	ObjectExpression_Binary::printDebug(out);
	*out << ")";
}

ObjectExpression::int_t ObjectExpression_BinaryAnd::resolveInt() const
{
	switch (getType())
	{
	case ObjectExpression::ET_FLOAT:
		return ObjectExpression_Binary::resolveInt();

	case ObjectExpression::ET_INT:
		return _exprL.resolveInt() & _exprR.resolveInt();
	}

	return ObjectExpression_Binary::resolveInt();
}



