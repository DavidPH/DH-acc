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

/* ObjectExpression/BinarySub.cpp
**
** ObjectExpressionBinarySub class and methods.
*/

#include "../ObjectExpression.hpp"



class ObjectExpressionBinarySub : public ObjectExpressionBase
{
public:
	ObjectExpressionBinarySub(ObjectExpression const & exprL, ObjectExpression const & exprR);

	virtual ObjectExpressionBinarySub * clone() const;

	virtual int32_t resolveInt32() const;

private:
	ObjectExpression _exprL;
	ObjectExpression _exprR;
};



ObjectExpression ObjectExpression::create_binary_sub(ObjectExpression const & exprL, ObjectExpression const & exprR)
{
	return new ObjectExpressionBinarySub(exprL, exprR);
}



ObjectExpressionBinarySub::ObjectExpressionBinarySub(ObjectExpression const & exprL, ObjectExpression const & exprR) : ObjectExpressionBase(exprL.getPosition()), _exprL(exprL), _exprR(exprR)
{

}

ObjectExpressionBinarySub * ObjectExpressionBinarySub::clone() const
{
	return new ObjectExpressionBinarySub(*this);
}

int32_t ObjectExpressionBinarySub::resolveInt32() const
{
	return _exprL.resolveInt32() - _exprR.resolveInt32();
}



