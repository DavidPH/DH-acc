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

/* ObjectExpression/UnarySub.cpp
**
** Defines the ObjectExpression_UnarySub class and methods.
*/

#include "Unary.hpp"



class ObjectExpression_UnarySub : public ObjectExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_UnarySub, ObjectExpression_Unary);

public:
	ObjectExpression_UnarySub(ObjectExpression * expr, SourcePosition const & position);

	virtual void printDebug(std::ostream * out) const;

	virtual float_t resolveFloat() const;
	virtual int_t resolveInt() const;
};



ObjectExpression::Pointer ObjectExpression::create_unary_sub(ObjectExpression * expr, SourcePosition const & position)
{
	return new ObjectExpression_UnarySub(expr, position);
}



ObjectExpression_UnarySub::ObjectExpression_UnarySub(ObjectExpression * expr, SourcePosition const & position) : ObjectExpression_Unary(expr, position)
{

}

void ObjectExpression_UnarySub::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_UnarySub(";
	Super::printDebug(out);
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_UnarySub::resolveFloat() const
{
	if (getType() == ET_FLOAT) return -expr->resolveFloat();

	return Super::resolveFloat();
}
ObjectExpression::int_t ObjectExpression_UnarySub::resolveInt() const
{
	if (getType() == ET_INT) return -expr->resolveInt();

	return Super::resolveInt();
}



