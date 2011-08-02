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

/* SourceExpression/BranchBreak.cpp
**
** Defines the SourceExpression_BranchBreak class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"



class SourceExpression_BranchBreak : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchBreak, SourceExpression);

public:
	SourceExpression_BranchBreak(SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	std::string _label;
};



SourceExpression::Pointer SourceExpression::create_branch_break(SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchBreak(context, position);
}



SourceExpression_BranchBreak::SourceExpression_BranchBreak(SourceContext * context, SourcePosition const & position) : Super(position), _label(context->getLabelBreak(position))
{

}

void SourceExpression_BranchBreak::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	objects->setPosition(position);

	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_label));
}

void SourceExpression_BranchBreak::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchBreak(";
	Super::printDebug(out);
	*out << " ";
		*out << "label=(";
		print_debug(out, _label);
		*out << ")";
	*out << ")";
}


