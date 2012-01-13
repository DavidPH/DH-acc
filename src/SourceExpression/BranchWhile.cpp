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

/* SourceExpression/BranchWhile.cpp
**
** Defines the SourceExpression_BranchWhile class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchWhile : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchWhile, SourceExpression);

public:
	SourceExpression_BranchWhile(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, bool postCondition, SourcePosition const & position);

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	SourceExpression::Pointer _exprCondition;
	SourceExpression::Pointer _exprWhile;

	std::string _labelBreak;
	std::string _labelContinue;
	std::string _labelLoop;

	bool _postCondition;
};



SourceExpression::Pointer SourceExpression::create_branch_do(SourceExpression * exprCondition, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchWhile(exprCondition, exprLoop, context, true, position);
}

SourceExpression::Pointer SourceExpression::create_branch_while(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchWhile(exprCondition, exprWhile, context, false, position);
}

SourceExpression_BranchWhile::SourceExpression_BranchWhile(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, bool postCondition, SourcePosition const & position_) : Super(position_), _exprCondition(exprCondition), _exprWhile(exprWhile), _labelBreak(context->getLabelBreak(position)), _labelContinue(context->getLabelContinue(position)), _labelLoop(context->makeLabel() + "_loop"), _postCondition(postCondition)
{
	if (_exprCondition->getType()->vt != VariableType::VT_BOOLSOFT)
		_exprCondition = create_value_cast(_exprCondition, VariableType::get_vt_boolsoft(), position);

	if (_exprWhile->getType()->vt != VariableType::VT_VOID)
		_exprWhile = create_value_cast(_exprWhile, VariableType::get_vt_void(), position);
}

void SourceExpression_BranchWhile::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	if (!_postCondition)
		objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(_labelContinue));

	objects->addLabel(_labelLoop);
	_exprWhile->makeObjects(objects, VariableData::create_void(0));

	objects->addLabel(_labelContinue);
	_exprCondition->makeObjects(objects, VariableData::create_stack(_exprCondition->getType()->size(position)));
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(_labelLoop));

	objects->addLabel(_labelBreak);
}

// EOF

