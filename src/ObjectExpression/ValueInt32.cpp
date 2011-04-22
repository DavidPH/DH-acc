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

/* ObjectExpression/ValueInt32.cpp
**
** ObjectExpressionValueInt32 class and methods.
*/

#include "../ObjectExpression.hpp"



class ObjectExpressionValueInt32 : public ObjectExpressionBase
{
public:
	ObjectExpressionValueInt32(int32_t const value, SourcePosition const & position);

	virtual ObjectExpressionValueInt32 * clone() const;

	virtual int32_t resolveInt32() const;

private:
	int32_t _value;
};



ObjectExpression ObjectExpression::create_value_int32(int32_t const value, SourcePosition const & position)
{
	return new ObjectExpressionValueInt32(value, position);
}



ObjectExpressionValueInt32::ObjectExpressionValueInt32(int32_t const value, SourcePosition const & position) : ObjectExpressionBase(position), _value(value)
{

}

ObjectExpressionValueInt32 * ObjectExpressionValueInt32::clone() const
{
	return new ObjectExpressionValueInt32(*this);
}

int32_t ObjectExpressionValueInt32::resolveInt32() const
{
	return _value;
}



