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

/* ObjectExpression/ValueSymbol.cpp
**
** ObjectExpressionValueSymbol class and methods.
*/

#include "../ObjectExpression.hpp"



class ObjectExpressionValueSymbol : public ObjectExpressionBase
{
public:
	ObjectExpressionValueSymbol(std::string const & value, SourcePosition const & position);

	virtual ObjectExpressionValueSymbol * clone() const;

	virtual int32_t resolveInt32() const;

private:
	std::string _value;
};



ObjectExpression ObjectExpression::create_value_symbol(std::string const & value, SourcePosition const & position)
{
	return new ObjectExpressionValueSymbol(value, position);
}



ObjectExpressionValueSymbol::ObjectExpressionValueSymbol(std::string const & value, SourcePosition const & position) : ObjectExpressionBase(position), _value(value)
{

}

ObjectExpressionValueSymbol * ObjectExpressionValueSymbol::clone() const
{
	return new ObjectExpressionValueSymbol(*this);
}

int32_t ObjectExpressionValueSymbol::resolveInt32() const
{
	return ObjectExpression::get_symbol(_value, _position);
}



