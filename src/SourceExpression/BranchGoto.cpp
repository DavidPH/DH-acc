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
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchGoto
//
class SourceExpression_BranchGoto : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchGoto,
                                   SourceExpression);

public:
   SourceExpression_BranchGoto(std::string const &label, SRCEXP_EXPR_ARGS);
   SourceExpression_BranchGoto(SRCEXP_EXPRUNA_ARGS);

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
   std::string labelGoto;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_break
//
SRCEXP_EXPRBRA_DEFN(0, break)
{
   return new SourceExpression_BranchGoto(context->getLabelBreak(position),
                                          context, position);
}

//
// SourceExpression::create_branch_continue
//
SRCEXP_EXPRBRA_DEFN(0, continue)
{
   return new SourceExpression_BranchGoto(context->getLabelContinue(position),
                                          context, position);
}

//
// SourceExpression::create_branch_goto
//
SRCEXP_EXPRBRA_DEFN(s, goto)
{
   return new SourceExpression_BranchGoto(value, context, position);
}


//
// SourceExpression::create_branch_goto
//
SRCEXP_EXPRBRA_DEFN(u, goto)
{
   return new SourceExpression_BranchGoto(expr, context, position);
}

//
// SourceExpression_BranchGoto::SourceExpression_BranchGoto
//
SourceExpression_BranchGoto::
SourceExpression_BranchGoto(std::string const &_label, SRCEXP_EXPR_PARM)
                            : Super(SRCEXP_EXPR_PASS),
                              labelGoto(_label)
{
}

//
// SourceExpression_BranchGoto::SourceExpression_BranchGoto
//
SourceExpression_BranchGoto::
SourceExpression_BranchGoto(SRCEXP_EXPRUNA_PARM)
                            : Super(SRCEXP_EXPR_PASS), expr(_expr)
{
   if (expr->getType()->vt != VariableType::VT_LABEL)
      expr = create_value_cast_implicit
             (expr, VariableType::get_vt_label(), context, position);
}

//
// SourceExpression_BranchGoto::virtual_makeObjects
//
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
		objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelGoto));
}

// EOF

