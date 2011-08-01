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

/* SourceExpression/Unary.cpp
**
** Defines the SourceExpression_Binary methods.
*/

#include "Unary.hpp"

#include "../ObjectVector.hpp"



SourceExpression_Unary::SourceExpression_Unary(SourceExpression * expr, SourcePosition const & position) : Super(position), expr(expr)
{

}

SourceVariable::VariableType const * SourceExpression_Unary::getType() const
{
	return expr->getType();
}

void SourceExpression_Unary::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	expr->makeObjectsGet(objects);
}

void SourceExpression_Unary::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_Unary(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, expr);
		*out << ")";
	*out << ")";
}



