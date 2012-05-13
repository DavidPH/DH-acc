//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// SourceExpression handling of "operator ^^" and "operator ^^=".
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchXOr
//
class SourceExpression_BranchXOr : public SourceExpression_BinaryCompare
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchXOr,
                                   SourceExpression_BinaryCompare);

public:
   SourceExpression_BranchXOr(SRCEXP_EXPRBIN_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_xor
//
SRCEXP_EXPRBRA_DEFN(b, xor)
{
   return new SourceExpression_BranchXOr(exprL, exprR, context, pos);
}

//
// SourceExpression::create_branch_xor_eq
//
SRCEXP_EXPRBRA_DEFN(b, xor_eq)
{
   return create_binary_assign
      (exprL, create_branch_xor(exprL, exprR, context, pos), context, pos);
}

//
// SourceExpression_BranchXOr::SourceExpression_BranchXOr
//
SourceExpression_BranchXOr::SourceExpression_BranchXOr(SRCEXP_EXPRBIN_PARM)
 : Super(SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BranchXOr::virtual_makeObjects
//
void SourceExpression_BranchXOr::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference srcType = VariableType::get_bt_bit_hrd();
   bigsint                 srcSize = srcType->getSize(pos);

   VariableData::Pointer src = VariableData::create_stack(srcSize);
   VariableData::Pointer tmp = VariableData::create_stack(srcSize);

   if (dst->type == VariableData::MT_VOID)
   {
      exprL->makeObjects(objects, dst);
      exprR->makeObjects(objects, dst);
   }
   else
   {
      std::string label = context->makeLabel();
      std::string labelL0  = label + "_L0";
      std::string label0   = label + "_0";
      std::string label1   = label + "_1";
      std::string labelEnd = label + "_end";

      make_objects_memcpy_prep(objects, dst, src, pos);

      exprL->makeObjects(objects, tmp);
      exprR->makeObjects(objects, tmp);

      objects->setPosition(pos);
      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(labelL0));

      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(label1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(label0));

      objects->addLabel(labelL0);
      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(label0));

      objects->addLabel(label1);
      objects->addToken(OCODE_GET_LITERAL32I,  objects->getValue(1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      objects->addLabel(label0);
      objects->addToken(OCODE_GET_LITERAL32I,  objects->getValue(0));

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, srcType, context, pos);
   }
}

// EOF

