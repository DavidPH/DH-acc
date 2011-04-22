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
** ObjectExpressionUnarySub class and methods.
*/

#include "../ObjectExpression.hpp"



class ObjectExpressionUnarySub : public ObjectExpressionBase
{
public:
	ObjectExpressionUnarySub(ObjectExpression const & expr);

	virtual ObjectExpressionUnarySub * clone() const;

	virtual int32_t resolveInt32() const;

private:
	ObjectExpression _expr;
};



ObjectExpression ObjectExpression::create_unary_sub(ObjectExpression const & expr)
{
	return new ObjectExpressionUnarySub(expr);
}



ObjectExpressionUnarySub::ObjectExpressionUnarySub(ObjectExpression const & expr) : ObjectExpressionBase(expr.getPosition()), _expr(expr)
{

}

ObjectExpressionUnarySub * ObjectExpressionUnarySub::clone() const
{
	return new ObjectExpressionUnarySub(*this);
}

int32_t ObjectExpressionUnarySub::resolveInt32() const
{
	return -_expr.resolveInt32();
}



