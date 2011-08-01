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

/* SourceExpression/ValueCast.cpp
**
** Defines the SourceExpression_ValueCast class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"



class SourceExpression_ValueCast : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueCast, SourceExpression);

public:
	SourceExpression_ValueCast(SourceExpression * expr, SourceVariable::VariableType const * type, SourcePosition const & postion);

	virtual bool canMakeObject() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _expr;
	SourceVariable::VariableType const * _type;
};



SourceExpression::Pointer SourceExpression::create_value_cast(SourceExpression * expr, SourceVariable::VariableType const * type, SourcePosition const & position)
{
	return new SourceExpression_ValueCast(expr, type, position);
}



SourceExpression_ValueCast::SourceExpression_ValueCast(SourceExpression * expr, SourceVariable::VariableType const * type, SourcePosition const & position) : Super(position), _expr(expr), _type(type)
{

}

bool SourceExpression_ValueCast::canMakeObject() const
{
	return _expr->canMakeObject();
}

SourceVariable::VariableType const * SourceExpression_ValueCast::getType() const
{
	return _type;
}

void SourceExpression_ValueCast::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);
	_expr->makeObjectsCast(objects, _type);
}

void SourceExpression_ValueCast::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_ValueCast(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, _type);
		*out << ")";
	*out << ")";
}



