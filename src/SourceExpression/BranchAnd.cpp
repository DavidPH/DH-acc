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
// SourceExpression handling of "operator &&".
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchAnd
//
class SourceExpression_BranchAnd : public SourceExpression_BinaryCompare
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchAnd,
                                   SourceExpression_BinaryCompare);

public:
   SourceExpression_BranchAnd(SRCEXP_EXPRBIN_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_and
//
SRCEXP_EXPRBRA_DEFN(b, and)
{
   return new SourceExpression_BranchAnd(exprL, exprR, context, position);
}

//
// SourceExpression_BranchAnd::SourceExpression_BranchAnd
//
SourceExpression_BranchAnd::
SourceExpression_BranchAnd(SRCEXP_EXPRBIN_PARM)
                           : Super(true, SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BranchAnd::virtual_makeObjects
//
void SourceExpression_BranchAnd::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType const *srcType = VariableType::get_vt_boolhard();
   bigsint             srcSize = srcType->size(position);

   VariableData::Pointer src = VariableData::create_stack(srcSize);
   VariableData::Pointer tmp = VariableData::create_stack(srcSize);

   std::string label0   = label + "_0";
   std::string labelEnd = label + "_end";

   if (dst->type == VariableData::MT_VOID)
   {
      exprL->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelEnd));

      exprR->makeObjects(objects, dst);

      objects->addLabel(labelEnd);
   }
   else
   {
      make_objects_memcpy_prep(objects, dst, src, position);

      exprL->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(label0));

      exprR->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(label0));

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      objects->addLabel(label0);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0));

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, position);
   }
}

// EOF

