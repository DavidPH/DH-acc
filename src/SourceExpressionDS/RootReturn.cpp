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

/* SourceExpressionDS/RootReturn.cpp
**
** Defines the SourceExpressionDS_RootReturn class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"
#include "../SourceContext.hpp"



class SourceExpressionDS_RootReturn : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootReturn(SourceExpressionDS const & expr, SourceContext const & context, SourcePosition const & position);

	virtual SourceExpressionDS_RootReturn * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
	SourceContext::ContextType _type;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_return(SourceExpressionDS const & expr, SourceContext const & context, SourcePosition const & position)
{
	return new SourceExpressionDS_RootReturn(expr, context, position);
}



SourceExpressionDS_RootReturn::SourceExpressionDS_RootReturn(SourceExpressionDS const & expr, SourceContext const & context, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr), _type(context.getTypeRoot())
{
	if (_expr.getType() != context.getReturnType())
		_expr = SourceExpressionDS::make_expression_cast(_expr, context.getReturnType(), getPosition());
}

SourceExpressionDS_RootReturn * SourceExpressionDS_RootReturn::clone() const
{
	return new SourceExpressionDS_RootReturn(*this);
}

char const * SourceExpressionDS_RootReturn::getName() const
{
	return "SourceExpressionDS_RootReturn";
}

SourceVariable::VariableType const * SourceExpressionDS_RootReturn::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootReturn::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootReturn::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_expr.makeObjectsGet(objects);

	switch (_type)
	{
	case SourceContext::CT_ACSFUNC:
		if (_expr.getType()->type == SourceVariable::VT_VOID)
			objects->push_back(ObjectToken(ObjectToken::OCODE_RETURNZDACSVOID, getPosition()));
		else
			objects->push_back(ObjectToken(ObjectToken::OCODE_RETURNZDACS, getPosition()));

		break;

	case SourceContext::CT_BLOCK:
		break;

	case SourceContext::CT_SCRIPT:
		if (_expr.getType()->type != SourceVariable::VT_VOID)
			objects->push_back(ObjectToken(ObjectToken::OCODE_SETRESULTVALUE, getPosition()));

		objects->push_back(ObjectToken(ObjectToken::OCODE_TERMINATE, getPosition()));

		break;
	}
}

void SourceExpressionDS_RootReturn::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootReturn(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



