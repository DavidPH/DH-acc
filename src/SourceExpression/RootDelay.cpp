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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_RootDelay : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootDelay, SourceExpression);

public:
	SourceExpression_RootDelay(SourceExpression * expr, SourceContext * context, SourcePosition const & position);

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	SourceExpression::Pointer _expr;
	bigsint _stack;
};



SourceExpression::Pointer SourceExpression::create_root_delay(SourceExpression * expr, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_RootDelay(expr, context, position);
}

SourceExpression_RootDelay::SourceExpression_RootDelay(SourceExpression * expr, SourceContext * context, SourcePosition const & position_) : Super(position_), _expr(expr), _stack(context->getLimit(SourceVariable::SC_AUTO))
{
	if (_expr->getType()->vt != VariableType::VT_INT)
		_expr = create_value_cast(_expr, VariableType::get_vt_int(), position);
}

void SourceExpression_RootDelay::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	_expr->makeObjects(objects, VariableData::create_stack(_expr->getType()->size(position)));

	objects->setPosition(position);

	ObjectExpression::Pointer stack(objects->getValue(_stack));
	objects->addToken(OCODE_ADDR_STACK_ADD_IMM, stack);
	objects->addToken(OCODE_MISC_DELAY32);
	objects->addToken(OCODE_ADDR_STACK_SUB_IMM, stack);
}

// EOF

