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

/* ObjectExpression/BinaryIOr.cpp
**
** Defines the ObjectExpression_BinaryIOr class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryIOr : public ObjectExpression_Binary
{
public:
	ObjectExpression_BinaryIOr(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position);

	virtual ObjectExpression_BinaryIOr * clone() const;

	virtual char const * getName() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::int_t resolveInt() const;
};



ObjectExpression ObjectExpression::create_binary_ior(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position)
{
	return ObjectExpression_BinaryIOr(exprL, exprR, position);
}



ObjectExpression_BinaryIOr::ObjectExpression_BinaryIOr(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position) : ObjectExpression_Binary(exprL, exprR, position)
{

}

ObjectExpression_BinaryIOr * ObjectExpression_BinaryIOr::clone() const
{
	return new ObjectExpression_BinaryIOr(*this);
}

char const * ObjectExpression_BinaryIOr::getName() const
{
	return "ObjectExpression_BinaryIOr";
}

void ObjectExpression_BinaryIOr::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_BinaryIOr(";
	ObjectExpression_Binary::printDebug(out);
	*out << ")";
}

ObjectExpression::int_t ObjectExpression_BinaryIOr::resolveInt() const
{
	switch (getType())
	{
	case ObjectExpression::ET_FLOAT:
		return ObjectExpression_Binary::resolveInt();

	case ObjectExpression::ET_INT:
		return _exprL.resolveInt() | _exprR.resolveInt();
	}

	return ObjectExpression_Binary::resolveInt();
}



