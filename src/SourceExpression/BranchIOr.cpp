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

/* SourceExpression/BranchIOr.cpp
**
** Defines the SourceExpression_BranchIOr class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchIOr : public SourceExpression_BinaryCompare
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchIOr, SourceExpression_BinaryCompare);

public:
	SourceExpression_BranchIOr(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpression::Pointer _exprL;
	SourceExpression::Pointer _exprR;

	std::string _label1;
	std::string _labelEnd;
};



SourceExpression::Pointer SourceExpression::create_branch_ior(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchIOr(exprL, exprR, context, position);
}



SourceExpression_BranchIOr::SourceExpression_BranchIOr(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position) : Super(exprL, exprR, position)
{
	std::string label(context->makeLabel());

	_label1   = label + "_1";
	_labelEnd = label + "_end";
}

void SourceExpression_BranchIOr::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	exprL->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHNOTZERO, objects->getValue(_label1));

	exprR->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHNOTZERO, objects->getValue(_label1));

	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelEnd));

	objects->addLabel(_label1);
	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));

	objects->addLabel(_labelEnd);
}

void SourceExpression_BranchIOr::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchIOr(";
	Super::printDebug(out);
	*out << ")";
}



