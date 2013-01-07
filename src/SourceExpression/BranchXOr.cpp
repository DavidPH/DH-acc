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
// SourceExpression handling of "operator ^^" and "operator ^^=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

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
class SourceExpression_BranchXOr : public SourceExpression_Binary
{
   CounterPreambleNoClone(SourceExpression_BranchXOr, SourceExpression_Binary);

public:
   SourceExpression_BranchXOr(SRCEXP_EXPRBIN_PARM) : Super(SRCEXP_EXPRBIN_PASS,
      VariableType::get_bt_bit_hrd(), VariableType::get_bt_bit_hrd(), false) {}

   VariableType::Reference getType() const {return VariableType::get_bt_bit_hrd();}

   //
   // makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return ObjectExpression::create_branch_xor(exprL->makeObject(), exprR->makeObject(), pos);
   }

private:
   //
   // virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      auto    srcType = VariableType::get_bt_bit_hrd();
      bigsint srcSize = srcType->getSize(pos);

      auto src = VariableData::create_stack(srcSize);
      auto tmp = VariableData::create_stack(srcSize);

      if(dst->type == VariableData::MT_VOID)
      {
         exprL->makeObjects(objects, dst);
         exprR->makeObjects(objects, dst);
      }
      else
      {
         make_objects_memcpy_prep(objects, dst, src, pos);

         exprL->makeObjects(objects, tmp);
         exprR->makeObjects(objects, tmp);
         objects->setPosition(pos);
         objects->addToken(OCODE_XOR_STK_I);

         make_objects_memcpy_post(objects, dst, src, srcType, context, pos);
      }
   }
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

// EOF

