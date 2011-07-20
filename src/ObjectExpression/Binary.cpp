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



ObjectExpression_Binary::ObjectExpression_Binary(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position) : ObjectExpression_Base(position), _exprL(exprL), _exprR(exprR)
{
	if (_exprL.getType() != _exprR.getType())
		throw SourceException("type mismatch", getPosition(), "ObjectExpression_Binary");
}

ObjectExpression::ExpressionType ObjectExpression_Binary::getType() const
{
	return _exprL.getType();
}

void ObjectExpression_Binary::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_Binary(";
	ObjectExpression_Base::printDebug(out);
	*out << " ";
		*out << "exprL=(";
		print_debug(out, _exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, _exprR);
		*out << ")";
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_Binary::resolveFloat() const
{
	throw SourceException("cannot resolve float", getPosition(), getName());
}
ObjectExpression::int_t ObjectExpression_Binary::resolveInt() const
{
	throw SourceException("cannot resolve int", getPosition(), getName());
}



