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

/* SourceExpression/BranchReturn.cpp
**
** Defines the SourceExpression_BranchReturn class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchReturn : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchReturn, SourceExpression);

public:
	SourceExpression_BranchReturn(SourceExpression * expr, SourceContext * context, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);

	SourceExpression::Pointer _expr;
	SourceContext::ContextType _type;
};



SourceExpression::Pointer SourceExpression::create_branch_return(SourceExpression * expr, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchReturn(expr, context, position);
}



SourceExpression_BranchReturn::SourceExpression_BranchReturn(SourceExpression * expr, SourceContext * context, SourcePosition const & position) : Super(position), _expr(expr), _type(context->getTypeRoot())
{
	if (_expr->getType() != context->getReturnType())
		_expr = SourceExpression::create_value_cast(_expr, context->getReturnType(), position);
}

void SourceExpression_BranchReturn::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchReturn(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}

void SourceExpression_BranchReturn::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	for (int i(1); i < _expr->getType()->size(position); ++i)
		objects->addToken(OCODE_ASSIGNSTACKVAR, objects->getValue(-i));

	switch (_type)
	{
	case SourceContext::CT_BLOCK:
		objects->addToken(OCODE_TERMINATE);
		break;

	case SourceContext::CT_FUNCTION:
		if (_expr->getType()->size(position) == 0)
			objects->addToken(OCODE_RETURNZDACSVOID);
		else
			objects->addToken(OCODE_RETURNZDACS);

		break;

	case SourceContext::CT_SCRIPT:
		if (_expr->getType()->size(position) != 0)
			objects->addToken(OCODE_SETRESULTVALUE);

		objects->addToken(OCODE_TERMINATE);

		break;

	default:
		throw SourceException("invalid CT", position, getName());
	}
}


