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
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchFor : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchFor, SourceExpression);

public:
	SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	SourceExpression::Pointer _exprInit;
	SourceExpression::Pointer _exprCond;
	SourceExpression::Pointer _exprIter;
	SourceExpression::Pointer _exprLoop;

	std::string _labelCond;
	std::string _labelLoop;

	std::string _labelBreak;
	std::string _labelContinue;
};



SourceExpression::Pointer SourceExpression::create_branch_for(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchFor(exprInit, exprCond, exprIter, exprLoop, context, position);
}



SourceExpression_BranchFor::SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position_) : Super(position_), _exprInit(exprInit), _exprCond(exprCond), _exprIter(exprIter), _exprLoop(exprLoop), _labelBreak(context->getLabelBreak(position)), _labelContinue(context->getLabelContinue(position))
{
	std::string label(context->makeLabel());
	_labelCond = label + "_cond";
	_labelLoop = label + "_loop";

	if (_exprInit->getType()->vt != VariableType::VT_VOID)
		_exprInit = create_value_cast(_exprInit, VariableType::get_vt_void(), position);

	if (_exprCond->getType()->vt != VariableType::VT_BOOLSOFT)
		_exprCond = create_value_cast(_exprCond, VariableType::get_vt_boolsoft(), position);

	if (_exprIter->getType()->vt != VariableType::VT_VOID)
		_exprIter = create_value_cast(_exprIter, VariableType::get_vt_void(), position);

	if (_exprLoop->getType()->vt != VariableType::VT_VOID)
		_exprLoop = create_value_cast(_exprLoop, VariableType::get_vt_void(), position);
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

void SourceExpression_BranchFor::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	VariableData::Pointer src = VariableData::create_stack(VariableType::get_vt_boolsoft()->size(position));
	VariableData::Pointer snk = VariableData::create_void(0);

	Super::recurse_makeObjects(objects, dst);

	_exprInit->makeObjects(objects, snk);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(_labelCond));

	objects->addLabel(_labelLoop);
	_exprLoop->makeObjects(objects, snk);

	objects->addLabel(_labelContinue);
	_exprIter->makeObjects(objects, snk);

	objects->addLabel(_labelCond);
	_exprCond->makeObjects(objects, src);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(_labelLoop));

	objects->addLabel(_labelBreak);
}


