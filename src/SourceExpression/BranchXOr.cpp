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

/* SourceExpression/BranchXOr.cpp
**
** Defines the SourceExpression_BranchXOr class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchXOr : public SourceExpression_BinaryCompare
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchXOr, SourceExpression_BinaryCompare);

public:
	SourceExpression_BranchXOr(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);

	SourceExpression::Pointer _exprL;
	SourceExpression::Pointer _exprR;

	std::string _labelL0;
	std::string _label0;
	std::string _label1;
	std::string _labelEnd;
};



SourceExpression::Pointer SourceExpression::create_branch_xor(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchXOr(exprL, exprR, context, position);
}



SourceExpression_BranchXOr::SourceExpression_BranchXOr(SourceExpression * exprL_, SourceExpression * exprR_, SourceContext * context, SourcePosition const & position_) : Super(exprL_, exprR_, true, position_)
{
	std::string label(context->makeLabel());

	_labelL0  = label + "_L0";
	_label0   = label + "_0";
	_label1   = label + "_1";
	_labelEnd = label + "_end";
}

void SourceExpression_BranchXOr::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchXOr(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BranchXOr::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	exprL->makeObjectsGet(objects);
	exprR->makeObjectsGet(objects);

	objects->setPosition(position);
	objects->addToken(OCODE_BRANCHZERO, objects->getValue(_labelL0));

	objects->addToken(OCODE_BRANCHZERO, objects->getValue(_label1));
	objects->addToken(OCODE_BRANCH_IMM, objects->getValue(_label0));

	objects->addLabel(_labelL0);
	objects->addToken(OCODE_BRANCHZERO, objects->getValue(_label0));

	objects->addLabel(_label1);
	objects->addToken(OCODE_PUSHNUMBER, objects->getValue(1));
	objects->addToken(OCODE_BRANCH_IMM, objects->getValue(_labelEnd));

	objects->addLabel(_label0);
	objects->addToken(OCODE_PUSHNUMBER, objects->getValue(0));

	objects->addLabel(_labelEnd);
}


