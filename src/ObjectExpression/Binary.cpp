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

/* ObjectExpression/Binary.cpp
**
** Defines the ObjectExpression_Binary methods.
*/

#include "Binary.hpp"

#include "../SourceException.hpp"



ObjectExpression_Binary::ObjectExpression_Binary(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR)
{
	if (exprL->getType() != exprR->getType())
		throw SourceException("type mismatch", position, getName());
}
ObjectExpression_Binary::ObjectExpression_Binary(std::istream * in) : Super(in)
{
	read_object(in, &exprL);
	read_object(in, &exprR);
}

ObjectExpression::ExpressionType ObjectExpression_Binary::getType() const
{
	return exprL->getType();
}

void ObjectExpression_Binary::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_Binary(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprL=(";
		print_debug(out, *exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, *exprR);
		*out << ")";
	*out << ")";
}

void ObjectExpression_Binary::writeObject(std::ostream * out) const
{
	Super::writeObject(out);

	write_object(out, exprL);
	write_object(out, exprR);
}



