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

/* SourceExpressionDS/RootDelay.cpp
**
** Defines the SourceExpressionDS_RootDelay class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"



class SourceExpressionDS_RootDelay : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootDelay(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_RootDelay * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_delay(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootDelay(expr, position);
}



SourceExpressionDS_RootDelay::SourceExpressionDS_RootDelay(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{
	if (_expr.getType()->type != SourceVariable::VT_INT)
		_expr = SourceExpressionDS::make_expression_cast(_expr, SourceVariable::get_VariableType(SourceVariable::VT_INT), getPosition());
}

SourceExpressionDS_RootDelay * SourceExpressionDS_RootDelay::clone() const
{
	return new SourceExpressionDS_RootDelay(*this);
}

char const * SourceExpressionDS_RootDelay::getName() const
{
	return "SourceExpressionDS_RootDelay";
}

SourceVariable::VariableType const * SourceExpressionDS_RootDelay::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootDelay::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootDelay::makeObjectsGet(ObjectVector * objects) const
{
	_expr.makeObjectsGet(objects);

	objects->setPosition(getPosition());

	objects->addToken(ObjectToken::OCODE_DELAY);
}

void SourceExpressionDS_RootDelay::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootDelay(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



