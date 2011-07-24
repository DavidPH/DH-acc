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

/* SourceExpressionDS/RootVoid.cpp
**
** Defines the SourceExpressionDS_RootVoid class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"



class SourceExpressionDS_RootVoid : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootVoid(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_RootVoid * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_void(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootVoid(expr, position);
}
SourceExpressionDS SourceExpressionDS::make_expression_root_void(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootVoid(expr, position);
}



SourceExpressionDS_RootVoid::SourceExpressionDS_RootVoid(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{

}

SourceExpressionDS_RootVoid * SourceExpressionDS_RootVoid::clone() const
{
	return new SourceExpressionDS_RootVoid(*this);
}

char const * SourceExpressionDS_RootVoid::getName() const
{
	return "SourceExpressionDS_RootVoid";
}

SourceVariable::VariableType const * SourceExpressionDS_RootVoid::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootVoid::isConstant() const
{
	return _expr.isConstant();
}

void SourceExpressionDS_RootVoid::makeObjectsGet(ObjectVector * objects) const
{
	_expr.makeObjectsGet(objects);

	objects->setPosition(getPosition());

	for (size_t i(_expr.getType()->size()); i--;)
		objects->addToken(ObjectToken::OCODE_DROP);
}

void SourceExpressionDS_RootVoid::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootVoid(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



