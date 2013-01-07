//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// SourceExpression handling of "operator ||" and "operator ||=".
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
// SourceExpression_BranchIOr
//
class SourceExpression_BranchIOr : public SourceExpression_BinaryCompare
{
   CounterPreambleNoClone(SourceExpression_BranchIOr, SourceExpression_BinaryCompare);

public:
   SourceExpression_BranchIOr(SRCEXP_EXPRBIN_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_ior
//
SRCEXP_EXPRBRA_DEFN(b, ior)
{
	return new SourceExpression_BranchIOr(exprL, exprR, context, pos);
}

//
// SourceExpression::create_branch_ior_eq
//
SRCEXP_EXPRBRA_DEFN(b, ior_eq)
{
   return create_binary_assign
      (exprL, create_branch_ior(exprL, exprR, context, pos), context, pos);
}

//
// SourceExpression_BranchIOr::SourceExpression_BranchIOr
//
SourceExpression_BranchIOr::SourceExpression_BranchIOr(SRCEXP_EXPRBIN_PARM)
 : Super(SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BranchIOr::virtual_makeObjects
//
void SourceExpression_BranchIOr::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference srcType = VariableType::get_bt_bit_hrd();
   bigsint                 srcSize = srcType->getSize(pos);

   VariableData::Pointer src = VariableData::create_stack(srcSize);
   VariableData::Pointer tmp = VariableData::create_stack(srcSize);

   std::string label = context->makeLabel();
   std::string label1   = label + "_1";
   std::string labelEnd = label + "_end";

   if (dst->type == VariableData::MT_VOID)
   {
      exprL->makeObjects(objects, tmp);
      objects->setPosition(pos);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd));

      exprR->makeObjects(objects, dst);
      objects->setPosition(pos);

      objects->addLabel(labelEnd);
   }
   else
   {
      make_objects_memcpy_prep(objects, dst, src, pos);

      exprL->makeObjects(objects, tmp);
      objects->setPosition(pos);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(label1));

      exprR->makeObjects(objects, tmp);
      objects->setPosition(pos);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(label1));

      objects->addToken(OCODE_GET_IMM, objects->getValue(0));
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

      objects->addLabel(label1);
      objects->addToken(OCODE_GET_IMM, objects->getValue(1));

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, srcType, context, pos);
   }
}

// EOF

