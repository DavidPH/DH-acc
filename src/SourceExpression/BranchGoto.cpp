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

/* SourceExpression/BranchGoto.cpp
**
** Defines the SourceExpression_BranchGoto class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchGoto : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchGoto, SourceExpression);

public:
	SourceExpression_BranchGoto(std::string const & label, SourcePosition const & position);
	SourceExpression_BranchGoto(SourceExpression *expr, SourcePosition const &position);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	SourceExpression::Pointer expr;
	std::string _label;
};



SourceExpression::Pointer SourceExpression::create_branch_break(SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchGoto(context->getLabelBreak(position), position);
}
SourceExpression::Pointer SourceExpression::create_branch_continue(SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchGoto(context->getLabelContinue(position), position);
}
SourceExpression::Pointer SourceExpression::create_branch_goto(std::string const & label, SourceContext *, SourcePosition const & position)
{
	return new SourceExpression_BranchGoto(label, position);
}
SourceExpression::Pointer SourceExpression::create_branch_goto(SourceExpression *expr, SourceContext *, SourcePosition const &position)
{
	return new SourceExpression_BranchGoto(expr, position);
}



SourceExpression_BranchGoto::SourceExpression_BranchGoto(std::string const & label, SourcePosition const & position_) : Super(position_), _label(label)
{

}
SourceExpression_BranchGoto::SourceExpression_BranchGoto(SourceExpression *_expr, SourcePosition const &_position) : Super(_position), expr(_expr)
{
	if (expr->getType()->vt != VariableType::VT_LABEL)
		expr = create_value_cast(expr, VariableType::get_vt_label(), position);
}

void SourceExpression_BranchGoto::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchGoto(";
	Super::printDebug(out);
	*out << " ";
		*out << "label=(";
		print_debug(out, _label);
		*out << ")";
	*out << ")";
}

void SourceExpression_BranchGoto::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	if (expr)
	{
		if (expr->canMakeObject())
		{
			objects->addToken(OCODE_BRANCH_GOTO_IMM, expr->makeObject());
		}
		else
		{
			VariableData::Pointer src = VariableData::create_stack(expr->getType()->size(position));

			expr->makeObjects(objects, src);
			objects->setPosition(position);
			objects->addToken(OCODE_BRANCH_GOTO);
		}
	}
	else
		objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(_label));
}

