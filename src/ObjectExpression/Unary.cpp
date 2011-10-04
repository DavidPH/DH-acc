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



ObjectExpression_Unary::ObjectExpression_Unary(ObjectExpression * expr_, SourcePosition const & position_) : Super(position_), expr(expr_)
{

}
ObjectExpression_Unary::ObjectExpression_Unary(std::istream * in) : Super(in)
{
	read_object(in, &expr);
}

ObjectExpression::ExpressionType ObjectExpression_Unary::getType() const
{
	return expr->getType();
}

void ObjectExpression_Unary::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_Unary(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, *expr);
		*out << ")";
	*out << ")";
}

void ObjectExpression_Unary::writeObject(std::ostream * out) const
{
	Super::writeObject(out);

	write_object(out, expr);
}


