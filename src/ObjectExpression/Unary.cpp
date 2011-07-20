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

/* ObjectExpression/Unary.cpp
**
** Defines the ObjectExpression_Unary methods.
*/

#include "Unary.hpp"

#include "../SourceException.hpp"



ObjectExpression_Unary::ObjectExpression_Unary(ObjectExpression const & expr, SourcePosition const & position) : ObjectExpression_Base(position), _expr(expr)
{

}

ObjectExpression::ExpressionType ObjectExpression_Unary::getType() const
{
	return _expr.getType();
}

void ObjectExpression_Unary::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_Unary(";
	ObjectExpression_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_Unary::resolveFloat() const
{
	throw SourceException("cannot resolve float", getPosition(), getName());
}
ObjectExpression::int_t ObjectExpression_Unary::resolveInt() const
{
	throw SourceException("cannot resolve int", getPosition(), getName());
}



