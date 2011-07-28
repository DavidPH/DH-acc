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

/* SourceExpression/RootDelay.cpp
**
** Defines the SourceExpression_RootDelay class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"



class SourceExpression_RootDelay : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootDelay, SourceExpression);

public:
	SourceExpression_RootDelay(SourceExpression * expr, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _expr;
};



SourceExpression::Pointer SourceExpression::create_root_delay(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_RootDelay(expr, position);
}



SourceExpression_RootDelay::SourceExpression_RootDelay(SourceExpression * expr, SourcePosition const & position) : Super(position), _expr(expr)
{
	if (_expr->getType()->type != SourceVariable::VT_INT)
		_expr = create_value_cast(_expr, SourceVariable::get_VariableType(SourceVariable::VT_INT), position);
}

void SourceExpression_RootDelay::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	objects->addToken(ObjectToken::OCODE_DELAY);
}

void SourceExpression_RootDelay::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_RootDelay(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



