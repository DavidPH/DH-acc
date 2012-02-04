//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// SourceExpression handling of for if/else blocks.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchIf
//
class SourceExpression_BranchIf : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchIf,
                                   SourceExpression);

public:
   SourceExpression_BranchIf
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SRCEXP_EXPR_ARGS);

   SourceExpression_BranchIf
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SourceExpression *exprElse, SRCEXP_EXPR_ARGS);

   virtual VariableType const *getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprCond;
   SourceExpression::Pointer exprBody;
   SourceExpression::Pointer exprElse;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_if
//
SRCEXP_EXPRBRA_DEFN(2, if)
{
   return new SourceExpression_BranchIf(exprCond, exprBody, NULL, context,
                                        position);
}

//
// SourceExpression::create_branch_if
//
SRCEXP_EXPRBRA_DEFN(3, if)
{
   return new SourceExpression_BranchIf(exprCond, exprBody, exprElse, context,
                                        position);
}

//
// SourceExpression_BranchIf::SourceExpression_BranchIf
//
SourceExpression_BranchIf::
SourceExpression_BranchIf
(SourceExpression *_exprCond, SourceExpression *_exprBody,
 SourceExpression *_exprElse, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   exprCond(_exprCond), exprBody(_exprBody), exprElse(_exprElse)
{
   {
      VariableType const *typeCond = exprCond->getType();
      VariableType const *type     = VariableType::get_vt_boolsoft();

      if (typeCond != type)
         exprCond = create_value_cast_implicit
                    (exprCond, type, context, position);
   }

   if (exprElse)
   {
      VariableType const *typeBody = exprBody->getType();
      VariableType const *typeElse = exprElse->getType();
      VariableType const *type     =
         get_promoted_type(typeBody, typeElse, position);

      if (typeBody != type)
         exprBody = create_value_cast_implicit
                    (exprBody, type, context, position);

      if (typeElse != type)
         exprElse = create_value_cast_implicit
                    (exprElse, type, context, position);
   }
   else
   {
      VariableType const *typeBody = exprBody->getType();
      VariableType const *type     = VariableType::get_vt_void();

      if (typeBody != type)
         exprBody = create_value_cast_implicit
                    (exprBody, type, context, position);
   }
}

//
// SourceExpression_BranchIf::getType
//
VariableType const *SourceExpression_BranchIf::getType() const
{
   return exprBody->getType();
}

//
// SourceExpression_BranchIf::virtual_makeObjects
//
void SourceExpression_BranchIf::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint               sizeCond = exprCond->getType()->size(position);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);

   std::string labelBody = label + "_body";
   std::string labelElse = label + "_else";
   std::string labelDone = label + "_done";

   exprCond->makeObjects(objects, destCond);
   objects->setPosition(position);
   objects->addToken(OCODE_BRANCH_ZERO,
                     objects->getValue(exprElse ? labelElse : labelDone));

   objects->addLabel(labelBody);
   exprBody->makeObjects(objects, dst);

   if (exprElse)
   {
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelDone));
      objects->addLabel(labelElse);
      exprElse->makeObjects(objects, dst);
   }

   objects->addLabel(labelDone);
}

// EOF

