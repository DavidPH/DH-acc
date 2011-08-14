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

/* SourceExpression/BranchFor.cpp
**
** Defines the SourceExpression_BranchFor class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchFor : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchFor, SourceExpression);

public:
	SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpression::Pointer _exprInit;
	SourceExpression::Pointer _exprCond;
	SourceExpression::Pointer _exprIter;
	SourceExpression::Pointer _exprLoop;

	std::string _labelCond;

	std::string _labelBreak;
	std::string _labelContinue;
};



SourceExpression::Pointer SourceExpression::create_branch_for(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchFor(exprInit, exprCond, exprIter, exprLoop, context, position);
}



SourceExpression_BranchFor::SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position) : Super(position), _exprInit(exprInit), _exprCond(exprCond), _exprIter(exprIter), _exprLoop(exprLoop), _labelBreak(context->getLabelBreak(position)), _labelContinue(context->getLabelContinue(position))
{
	_labelCond = context->makeLabel() + "_cond";

	if (_exprInit->getType()->type != SourceVariable::VT_VOID)
		_exprInit = create_value_cast(_exprInit, SourceVariable::get_VariableType(SourceVariable::VT_VOID), position);

	if (_exprCond->getType()->type != SourceVariable::VT_BOOLSOFT)
		_exprCond = create_value_cast(_exprCond, SourceVariable::get_VariableType(SourceVariable::VT_BOOLSOFT), position);

	if (_exprIter->getType()->type != SourceVariable::VT_VOID)
		_exprIter = create_value_cast(_exprIter, SourceVariable::get_VariableType(SourceVariable::VT_VOID), position);

	if (_exprLoop->getType()->type != SourceVariable::VT_VOID)
		_exprLoop = create_value_cast(_exprLoop, SourceVariable::get_VariableType(SourceVariable::VT_VOID), position);
}

void SourceExpression_BranchFor::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_exprInit->makeObjectsGet(objects);

	objects->addLabel(_labelCond);
	_exprCond->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCHZERO, objects->getValue(_labelBreak));

	_exprLoop->makeObjectsGet(objects);

	objects->addLabel(_labelContinue);
	_exprIter->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCH, objects->getValue(_labelCond));

	objects->addLabel(_labelBreak);
}

void SourceExpression_BranchFor::printDebug(std::ostream * const out) const
{
	*out << "SourceExpression_BranchFor(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprInit=(";
		print_debug(out, _exprInit);
		*out << ")";

		*out << ", ";

		*out << "exprCond=(";
		print_debug(out, _exprCond);
		*out << ")";

		*out << ", ";

		*out << "exprIter=(";
		print_debug(out, _exprIter);
		*out << ")";

		*out << ", ";

		*out << "exprLoop=(";
		print_debug(out, _exprLoop);
		*out << ")";
	*out << ")";
}



