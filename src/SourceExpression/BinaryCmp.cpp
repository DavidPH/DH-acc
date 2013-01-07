//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// SourceExpression handling of relational operators.
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
// SourceExpression_BinaryCmp
//
class SourceExpression_BinaryCmp : public SourceExpression_Binary
{
   CounterPreambleNoClone(SourceExpression_BinaryCmp, SourceExpression_Binary);

public:
   enum CmpType
   {
      CMP_GE,
      CMP_GT,
      CMP_LE,
      CMP_LT,
      CMP_EQ,
      CMP_NE
   };

   //
   // ::SourceExpression_BinaryCmp
   //
   SourceExpression_BinaryCmp(CmpType _ct, SRCEXP_EXPRBIN_PARM)
    : Super(SRCEXP_EXPRBIN_PASS), ct(_ct)
   {
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      return false;
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      return VariableType::get_bt_bit_hrd();
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      ObjectExpression::Pointer objR;
      if (exprR->canMakeObject() && !(objR = exprR->makeObject())->canResolve())
         objR = NULL;

      VariableType::Reference inType = Super::getType();
      bigsint                 inSize = inType->getSize(pos);
      VariableType::BasicType inBT = inType->getBasicType();
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(pos);
      VariableData::Pointer   src  = VariableData::create_stack(size);
      VariableData::Pointer   tmp  = VariableData::create_stack(inSize);

      ObjectCode ocode = OCODE_NONE;
      switch (ct)
      {
      case CMP_GE: ocode = OCODE_CMP_GE_I; break;
      case CMP_GT: ocode = OCODE_CMP_GT_I; break;
      case CMP_LE: ocode = OCODE_CMP_LE_I; break;
      case CMP_LT: ocode = OCODE_CMP_LT_I; break;
      case CMP_EQ: ocode = OCODE_CMP_EQ_I; break;
      case CMP_NE: ocode = OCODE_CMP_NE_I; break;
      }

      make_objects_memcpy_prep(objects, dst, src, pos);

      // long long < 0 and long long >= 0 can be optimized by only checking the
      // high byte.
      if((inBT == VariableType::BT_INT_L || inBT == VariableType::BT_INT_LL) &&
         (ct == CMP_LT || ct == CMP_GE) && objR && !objR->resolveINT())
      {
         ObjectExpression::Pointer tmpH = context->getTempVar(0);

         exprL->makeObjects(objects, tmp);
         objects->addToken(OCODE_SET_TEMP, tmpH);
         objects->addToken(OCODE_STK_DROP);
         objects->addToken(OCODE_GET_TEMP, tmpH);
         objects->addToken(OCODE_GET_IMM, objR);
         objects->addToken(ocode);

         make_objects_memcpy_post(objects, dst, src, type, context, pos);
         return;
      }

      exprL->makeObjects(objects, tmp);
      exprR->makeObjects(objects, tmp);

      if(inBT == VariableType::BT_INT_L || inBT == VariableType::BT_INT_LL ||
         inBT == VariableType::BT_UNS_L || inBT == VariableType::BT_UNS_LL ||
         (inBT == VariableType::BT_PTR && inType->getSize(pos) == 2))
         return makeObjectsL(objects, ocode, dst, src, type, inBT);

      if (ct < CMP_EQ && VariableType::IsTypeUnsigned(inBT))
         return makeObjectsU(objects, ocode, dst, src, type, NULL, NULL);

      objects->addToken(ocode);

      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }

private:
   //
   // ::makeObjectsL
   //
   void makeObjectsL(ObjectVector *objects, ObjectCode ocode, VariableData *dst,
                     VariableData *src, VariableType *type, VariableType::BasicType inBT)
   {
      // Don't always need Jl, so don't allocate it yet.
      ObjectExpression::Pointer tmpJl;
      ObjectExpression::Pointer tmpJh = context->getTempVar(1);
      ObjectExpression::Pointer tmpKl = context->getTempVar(2);
      ObjectExpression::Pointer tmpKh = context->getTempVar(3);

      // llbits_t j; llbits_t k;
      objects->addToken(OCODE_SET_REG, tmpKh);
      objects->addToken(OCODE_SET_REG, tmpKl);
      objects->addToken(OCODE_SET_REG, tmpJh);

      if (ct == CMP_EQ)
      {
         // (j.lo == k.lo) & (j.hi == k.hi)
         objects->addToken(OCODE_GET_REG, tmpKl);
         objects->addToken(OCODE_CMP_EQ_I);

         objects->addToken(OCODE_GET_REG, tmpJh);
         objects->addToken(OCODE_GET_REG, tmpKh);
         objects->addToken(OCODE_CMP_EQ_I);

         objects->addToken(OCODE_AND_STK_I);
      }
      else if(ct == CMP_NE)
      {
         // (j.lo != k.lo) | (j.hi != k.hi)
         objects->addToken(OCODE_GET_REG, tmpKl);
         objects->addToken(OCODE_CMP_NE_I);

         objects->addToken(OCODE_GET_REG, tmpJh);
         objects->addToken(OCODE_GET_REG, tmpKh);
         objects->addToken(OCODE_CMP_NE_I);

         objects->addToken(OCODE_IOR_STK_I);
      }
      else
      {
         std::string label = context->makeLabel();
         std::string labelEnd = label + "_end";
         std::string labelLow = label + "_low";

         tmpJl = context->getTempVar(0);

         objects->addToken(OCODE_SET_REG, tmpJl);

         objects->addToken(OCODE_GET_REG, tmpJh);
         objects->addToken(OCODE_GET_REG, tmpKh);
         objects->addToken(OCODE_CMP_EQ_I);
         objects->addToken(OCODE_JMP_TRU, objects->getValue(labelLow));

         // The two high byes differ, so they determine the result.
         if(inBT == VariableType:: BT_INT_L || inBT == VariableType::BT_INT_LL)
         {
            objects->addToken(OCODE_GET_REG, tmpJh);
            objects->addToken(OCODE_GET_REG, tmpKh);
            objects->addToken(ocode);
         }
         else
            makeObjectsU(objects, ocode, dst, src, type, tmpJh, tmpKh);

         objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

         // Compare the low bytes. Low bytes are always effectively unsigned.
         objects->addLabel(labelLow);
         makeObjectsU(objects, ocode, dst, src, type, tmpJl, tmpKl);

         objects->addLabel(labelEnd);
      }

      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }

   //
   // ::makeObjectsU
   //
   void makeObjectsU
   (ObjectVector *objects, ObjectCode ocode, VariableData *dst,
    VariableData *src, VariableType *type, ObjectExpression::Pointer tmpJ,
    ObjectExpression::Pointer tmpK)
   {
      std::string label = context->makeLabel();
      std::string labelCmp = label + "_cmp";
      std::string labelEnd = label + "_end";
      std::string labelPos = label + "_pos";

      if (!tmpJ)
      {
         tmpJ = context->getTempVar(0);
         tmpK = context->getTempVar(1);

         // unsigned j; unsigned k;
         objects->addToken(OCODE_SET_REG, tmpK);
         objects->addToken(OCODE_SET_REG, tmpJ);
      }

      // if (j & 0x80000000) ...
      objects->addToken(OCODE_GET_REG, tmpJ);
      objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
      objects->addToken(OCODE_AND_STK_I);
      objects->addToken(OCODE_JMP_NIL, objects->getValue(labelPos));

      // ... then
      //    if (!(k & 0x80000000)) return +1;
      objects->addToken(OCODE_GET_REG, tmpK);
      objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
      objects->addToken(OCODE_AND_STK_I);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(labelCmp));
      if (ct == CMP_GE || ct == CMP_GT)
         objects->addToken(OCODE_GET_IMM, objects->getValue(1));
      else
         objects->addToken(OCODE_GET_IMM, objects->getValue(0));
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

      // ... else
      objects->addLabel(labelPos);
      //    if (k & 0x80000000) return -1;
      objects->addToken(OCODE_GET_REG, tmpK);
      objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
      objects->addToken(OCODE_AND_STK_I);
      objects->addToken(OCODE_JMP_NIL, objects->getValue(labelCmp));
      if (ct == CMP_LE || ct == CMP_LT)
         objects->addToken(OCODE_GET_IMM, objects->getValue(1));
      else
         objects->addToken(OCODE_GET_IMM, objects->getValue(0));
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

      // if (j < k) return -1;
      // if (j > k) return +1;
      objects->addLabel(labelCmp);
      objects->addToken(OCODE_GET_REG, tmpJ);
      objects->addToken(OCODE_GET_REG, tmpK);
      objects->addToken(ocode);

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }

   CmpType ct;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_cmp_ge
//
SRCEXP_EXPRBIN_DEFN(cmp_ge)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_GE, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_cmp_gt
//
SRCEXP_EXPRBIN_DEFN(cmp_gt)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_GT, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_cmp_le
//
SRCEXP_EXPRBIN_DEFN(cmp_le)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_LE, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_cmp_lt
//
SRCEXP_EXPRBIN_DEFN(cmp_lt)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_LT, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_cmp_eq
//
SRCEXP_EXPRBIN_DEFN(cmp_eq)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_EQ, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_cmp_ne
//
SRCEXP_EXPRBIN_DEFN(cmp_ne)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_NE, exprL, exprR, context, pos);
}

// EOF

