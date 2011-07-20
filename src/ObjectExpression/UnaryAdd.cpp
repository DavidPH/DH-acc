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

/* ObjectExpression/UnaryAdd.cpp
**
** Defines the ObjectExpression_UnaryAdd class and methods.
*/

#include "Unary.hpp"



class ObjectExpression_UnaryAdd : public ObjectExpression_Unary
{
public:
	ObjectExpression_UnaryAdd(ObjectExpression const & expr, SourcePosition const & position);

	virtual ObjectExpression_UnaryAdd * clone() const;

	virtual char const * getName() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const;
	virtual ObjectExpression::int_t resolveInt() const;
};



ObjectExpression ObjectExpression::create_unary_add(ObjectExpression const & expr, SourcePosition const & position)
{
	return ObjectExpression_UnaryAdd(expr, position);
}



ObjectExpression_UnaryAdd::ObjectExpression_UnaryAdd(ObjectExpression const & expr, SourcePosition const & position) : ObjectExpression_Unary(expr, position)
{

}

ObjectExpression_UnaryAdd * ObjectExpression_UnaryAdd::clone() const
{
	return new ObjectExpression_UnaryAdd(*this);
}

char const * ObjectExpression_UnaryAdd::getName() const
{
	return "ObjectExpression_UnaryAdd";
}

void ObjectExpression_UnaryAdd::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_UnaryAdd(";
	ObjectExpression_Unary::printDebug(out);
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_UnaryAdd::resolveFloat() const
{
	return _expr.resolveFloat();
}
ObjectExpression::int_t ObjectExpression_UnaryAdd::resolveInt() const
{
	return _expr.resolveInt();
}



