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

/* SourceExpression/BranchAnd.cpp
**
** Defines the SourceExpression_BranchNot class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchAnd : public SourceExpression_BinaryCompare
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchAnd, SourceExpression_BinaryCompare);

public:
	SourceExpression_BranchAnd(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::string _label0;
	std::string _labelEnd;
};



SourceExpression::Pointer SourceExpression::create_branch_and(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchAnd(exprL, exprR, context, position);
}



SourceExpression_BranchAnd::SourceExpression_BranchAnd(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position) : Super(exprL, exprR, position)
{
	std::string label(context->makeLabel());

	_label0   = label + "_0";
	_labelEnd = label + "_end";
}

void SourceExpression_BranchAnd::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	exprL->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_label0));

	exprR->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_label0));

	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));
	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelEnd));

	objects->addLabel(_label0);
	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));

	objects->addLabel(_labelEnd);
}

void SourceExpression_BranchAnd::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchAnd(";
	Super::printDebug(out);
	*out << ")";
}



