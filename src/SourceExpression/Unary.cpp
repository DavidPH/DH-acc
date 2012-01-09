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
#include "../VariableData.hpp"
#include "../VariableType.hpp"



SourceExpression_Unary::SourceExpression_Unary(SourceExpression * expr_, SourcePosition const & position_) : Super(position_), expr(expr_), _make(false)
{

}
SourceExpression_Unary::SourceExpression_Unary(SourceExpression * expr_, VariableType const * cast, SourcePosition const & position_) : Super(position_), expr(expr_), _make(true)
{
	if (cast && expr->getType() != cast)
		this->expr = create_value_cast(expr, cast, position);
}

VariableType const * SourceExpression_Unary::getType() const
{
	return expr->getType();
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

void SourceExpression_Unary::recurse_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	// Special case, child handles expression.
	if (!_make) return;

	VariableData::Pointer src = VariableData::create_stack(getType()->size(position));

	make_objects_memcpy_prep(objects, dst, src, position);
	expr->makeObjects(objects, src);

	objects->setPosition(position);
}


