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

/* SourceExpression/BranchContinue.cpp
**
** Defines the SourceExpression_BranchContinue class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchContinue : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchContinue, SourceExpression);

public:
	SourceExpression_BranchContinue(SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	std::string _label;
};



SourceExpression::Pointer SourceExpression::create_branch_continue(SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchContinue(context, position);
}



SourceExpression_BranchContinue::SourceExpression_BranchContinue(SourceContext * context, SourcePosition const & position) : Super(position), _label(context->getLabelContinue(position))
{

}

void SourceExpression_BranchContinue::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	objects->setPosition(position);

	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_label));
}

void SourceExpression_BranchContinue::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchContinue(";
	Super::printDebug(out);
	*out << " ";
		*out << "label=(";
		print_debug(out, _label);
		*out << ")";
	*out << ")";
}



