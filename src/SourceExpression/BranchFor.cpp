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

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchFor : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchFor, SourceExpression);

public:
	SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpression::Pointer _exprInit;
	SourceExpression::Pointer _exprCond;
	SourceExpression::Pointer _exprIter;
	SourceExpression::Pointer _exprLoop;

	std::string _labelCond;
	std::string _labelIter;

	std::string _labelBreak;
	std::string _labelContinue;
};



SourceExpression::Pointer SourceExpression::create_branch_for(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchFor(exprInit, exprCond, exprIter, exprLoop, context, position);
}



SourceExpression_BranchFor::SourceExpression_BranchFor(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position) : Super(position), _exprInit(exprInit), _exprCond(exprCond), _exprIter(exprIter), _exprLoop(exprLoop), _labelBreak(context->getLabelBreak(position)), _labelContinue(context->getLabelContinue(position))
{
	std::string label(context->makeLabel());

	_labelCond = label + "_cond";
	_labelIter = label + "_iter";
}

void SourceExpression_BranchFor::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	_exprInit->makeObjectsGet(objects);

	objects->addLabel(_labelCond);
	_exprCond->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_labelBreak));

	_exprLoop->makeObjectsGet(objects);

	objects->addLabel(_labelContinue);
	objects->addLabel(_labelIter);
	_exprIter->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelCond));

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



