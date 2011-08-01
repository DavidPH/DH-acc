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

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR)
{
	SourceVariable::VariableType const * type(getType());

	if (type->type == SourceVariable::VT_POINTER)
		return;

	if (exprL->getType() != type)
		this->exprL = create_value_cast(exprL, type, position);

	if (exprR->getType() != type)
		this->exprR = create_value_cast(exprR, type, position);
}
SourceExpression_Binary::SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, bool castL, SourcePosition const & position) : Super(position), exprL(exprL), exprR(exprR)
{
	if (exprL->getType() != exprR->getType())
	{
		if (castL)
		{
			this->exprL = create_value_cast(exprL, exprR->getType(), position);
		}
		else
		{
			this->exprR = create_value_cast(exprR, exprL->getType(), position);
		}
	}
}

SourceVariable::VariableType const * SourceExpression_Binary::getType() const
{
	return get_promoted_type(exprL->getType(), exprR->getType(), position);
}

void SourceExpression_Binary::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	SourceVariable::VariableType const * type(getType());

	// Pointer arithmetic.
	if (type->type == SourceVariable::VT_POINTER)
	{
		SourceVariable::VariableType const * typeL(exprL->getType());
		SourceVariable::VariableType const * typeR(exprR->getType());

		if (typeL->type == typeR->type && typeL != typeR)
			throw SourceException("VT_POINTER mismatch", position, getName());

		exprL->makeObjectsGet(objects);
		if (typeL->type != SourceVariable::VT_POINTER)
		{
			make_objects_cast(objects, typeL, type, position);
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->refType->size()));
			objects->addToken(ObjectToken::OCODE_MUL);
		}

		exprR->makeObjectsGet(objects);
		if (typeR->type != SourceVariable::VT_POINTER)
		{
			make_objects_cast(objects, typeR, type, position);
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->refType->size()));
			objects->addToken(ObjectToken::OCODE_MUL);
		}
	}
	else
	{
		exprL->makeObjectsGet(objects);
		exprR->makeObjectsGet(objects);
	}
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



