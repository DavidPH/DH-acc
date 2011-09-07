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

/* SourceExpression/Binary.cpp
**
** Defines the SourceExpression_Binary methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR), _arithmetic(2)
{
	doCast();
}
SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, bool arithmetic, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR), _arithmetic(arithmetic)
{
	doCast();
}
SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, VariableType const * castL, VariableType const * castR, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR), _arithmetic(2)
{
	doCast(castL, castR);
}
SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, VariableType const * castL, VariableType const * castR, bool arithmetic, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR), _arithmetic(arithmetic)
{
	doCast(castL, castR);
}

bool SourceExpression_Binary::canMakeObject() const
{
	return exprL->canMakeObject() && exprR->canMakeObject();
}

void SourceExpression_Binary::doCast()
{
	VariableType const * type(getType());

	// Pointer arithmetic.
	if (type->vt == VariableType::VT_POINTER && _arithmetic)
	{
		VariableType const * typeL(exprL->getType());
		VariableType const * typeR(exprR->getType());

		if (typeL->vt == typeR->vt && typeL != typeR)
			throw SourceException("VT_POINTER mismatch", position, getName());

		if (typeL->vt != VariableType::VT_POINTER)
		{
			if (type->refType->size() != 1)
			{
				if (typeL->vt != VariableType::VT_INT)
					exprL = create_value_cast(exprL, VariableType::get_vt_int(), position);

				exprL = create_binary_mul(exprL, create_value_int(type->refType->size(), position), position);
			}

			exprL = create_value_cast(exprL, type, position);
		}

		if (typeR->vt != VariableType::VT_POINTER)
		{
			if (type->refType->size() != 1)
			{
				if (typeR->vt != VariableType::VT_INT)
					exprR = create_value_cast(exprR, VariableType::get_vt_int(), position);

				exprR = create_binary_mul(exprR, create_value_int(type->refType->size(), position), position);
			}

			exprR = create_value_cast(exprR, type, position);
		}

		return;
	}

	if (exprL->getType() != type)
		exprL = create_value_cast(exprL, type, position);

	if (exprR->getType() != type)
		exprR = create_value_cast(exprR, type, position);
}
void SourceExpression_Binary::doCast(VariableType const * castL, VariableType const * castR)
{
	if (castL && exprL->getType() != castL)
		exprL = create_value_cast(exprL, castL, position);

	if (castR && exprR->getType() != castR)
		exprR = create_value_cast(exprR, castR, position);
}

VariableType const * SourceExpression_Binary::getType() const
{
	return get_promoted_type(exprL->getType(), exprR->getType(), position);
}

void SourceExpression_Binary::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_Binary(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprL=(";
		print_debug(out, exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, exprR);
		*out << ")";
	*out << ")";
}

void SourceExpression_Binary::recurse_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	// Special case, child handles expressions.
	if (_arithmetic == 2) return;

	exprL->makeObjectsGet(objects);
	exprR->makeObjectsGet(objects);

	objects->setPosition(position);
}



