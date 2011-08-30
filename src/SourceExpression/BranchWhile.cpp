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
#include "../VariableType.hpp"



class SourceExpression_BranchWhile : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchWhile, SourceExpression);

public:
	SourceExpression_BranchWhile(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);

	SourceExpression::Pointer _exprCondition;
	SourceExpression::Pointer _exprWhile;

	std::string _labelBreak;
	std::string _labelContinue;
};



SourceExpression::Pointer SourceExpression::create_branch_while(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchWhile(exprCondition, exprWhile, context, position);
}



SourceExpression_BranchWhile::SourceExpression_BranchWhile(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position) : Super(position), _exprCondition(exprCondition), _exprWhile(exprWhile), _labelBreak(context->getLabelBreak(position)), _labelContinue(context->getLabelContinue(position))
{
	if (_exprCondition->getType()->vt != VariableType::VT_BOOLSOFT)
		_exprCondition = create_value_cast(_exprCondition, VariableType::get_vt_boolsoft(), position);

	if (_exprWhile->getType()->vt != VariableType::VT_VOID)
		_exprWhile = create_value_cast(_exprWhile, VariableType::get_vt_void(), position);
}

void SourceExpression_BranchWhile::printDebug(std::ostream * const out) const
{
	*out << "SourceExpression_BranchWhile(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprCondition=(";
		print_debug(out, _exprCondition);
		*out << ")";

		*out << ", ";

		*out << "exprWhile=(";
		print_debug(out, _exprWhile);
		*out << ")";
	*out << ")";
}

void SourceExpression_BranchWhile::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	objects->addLabel(_labelContinue);
	_exprCondition->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCHZERO, objects->getValue(_labelBreak));

	_exprWhile->makeObjectsGet(objects);
	objects->addToken(OCODE_BRANCH, objects->getValue(_labelContinue));

	objects->addLabel(_labelBreak);
}


