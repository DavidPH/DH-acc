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
// SourceExpression handling of type casting.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define BAD_CAST() Error_P("bad cast: %s to %s", \
   make_string(srcType).c_str(), make_string(dstType).c_str())


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// ExtendSign
//
static void ExtendSign(ObjectVector *objects, biguint dstSize, biguint &srcSize,
                       SourceContext *context)
{
   if(srcSize < dstSize)
   {
      // Must sign-extend. Which means... BRANCHING!
      std::string label = context->makeLabel();
      std::string labelPos = label + "_pos";
      std::string labelEnd = label + "_end";

      objects->addToken(OCODE_STK_COPY);
      objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
      objects->addToken(OCODE_AND_STK_I);
      objects->addToken(OCODE_JMP_NIL, objects->getValue(labelPos));

      // Negative.
      for(biguint i = srcSize; i < dstSize; ++i)
         objects->addToken(OCODE_GET_IMM, objects->getValue(0xFFFFFFFF));
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

      // Positive.
      objects->addLabel(labelPos);
      for(biguint i = srcSize; i < dstSize; ++i)
         objects->addToken(OCODE_GET_IMM, objects->getValue(0x00000000));

      objects->addLabel(labelEnd);

      srcSize = dstSize;
   }
}

//
// GetET
//
static ObjectExpression::ExpressionType GetET(VariableType const *type)
{
   switch(type->getBasicType())
   {
   case VariableType::BT_VOID:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_BIT_SFT:
   case VariableType::BT_CHR:
      return ObjectExpression::ET_INT;

   case VariableType::BT_INT_HH:
      return ObjectExpression::ET_INT_HH;
   case VariableType::BT_INT_H:
      return ObjectExpression::ET_INT_H;
   case VariableType::BT_INT:
      return ObjectExpression::ET_INT;
   case VariableType::BT_INT_L:
      return ObjectExpression::ET_INT_L;
   case VariableType::BT_INT_LL:
      return ObjectExpression::ET_INT_LL;

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
   case VariableType::BT_SAT:
      return ObjectExpression::ET_MAP;

   case VariableType::BT_ACC_HH:
   case VariableType::BT_ACC_H:
   case VariableType::BT_ACC:
   case VariableType::BT_ACC_L:
   case VariableType::BT_ACC_LL:
   case VariableType::BT_ANG_HH:
   case VariableType::BT_ANG_H:
   case VariableType::BT_ANG:
   case VariableType::BT_ANG_L:
   case VariableType::BT_ANG_LL:
   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FRA_HH:
   case VariableType::BT_FRA_H:
   case VariableType::BT_FRA:
   case VariableType::BT_FRA_L:
   case VariableType::BT_FRA_LL:
      return ObjectExpression::ET_FIX;

   case VariableType::BT_FLT_HH:
      return ObjectExpression::ET_FLT_HH;
   case VariableType::BT_FLT_H:
      return ObjectExpression::ET_FLT_H;
   case VariableType::BT_FLT:
      return ObjectExpression::ET_FLT;
   case VariableType::BT_FLT_L:
      return ObjectExpression::ET_FLT_L;
   case VariableType::BT_FLT_LL:
      return ObjectExpression::ET_FLT_LL;

   case VariableType::BT_UNS_HH:
      return ObjectExpression::ET_UNS_HH;
   case VariableType::BT_UNS_H:
      return ObjectExpression::ET_UNS_H;
   case VariableType::BT_UNS:
      return ObjectExpression::ET_UNS;
   case VariableType::BT_UNS_L:
      return ObjectExpression::ET_UNS_L;
   case VariableType::BT_UNS_LL:
      return ObjectExpression::ET_UNS_LL;

   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_ARR:
      return ObjectExpression::ET_ARR;

   case VariableType::BT_PTR:
      if(type->getSize(SourcePosition::none()) == 2)
         return ObjectExpression::ET_UNS_L;
      else
         return ObjectExpression::ET_UNS;

   case VariableType::BT_PTR_NUL:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_ENUM:
      return ObjectExpression::ET_INT;

   case VariableType::BT_STRUCT:
      return ObjectExpression::ET_MAP;

   case VariableType::BT_UNION:
      // FIXME: wat
      return ObjectExpression::ET_UNS;

   case VariableType::BT_BLOCK:
      return ObjectExpression::ET_ARR;

   case VariableType::BT_FUN_ASM:
      return ObjectExpression::ET_OCS;

   case VariableType::BT_FUN:
   case VariableType::BT_FUN_LIN:
   case VariableType::BT_FUN_NAT:
   case VariableType::BT_FUN_SNA:
   case VariableType::BT_FUN_SNU:
      return ObjectExpression::ET_UNS;
   }

   return ObjectExpression::ET_UNS;
}

//
// GetSimpleET
//
static ObjectExpression::ExpressionType GetSimpleET(ObjectExpression::ExpressionType et)
{
   switch(et)
   {
   case ObjectExpression::ET_FIX_HH:
   case ObjectExpression::ET_FIX_H:
   case ObjectExpression::ET_FIX:
   case ObjectExpression::ET_FIX_L:
   case ObjectExpression::ET_FIX_LL:
      return ObjectExpression::ET_FIX;

   case ObjectExpression::ET_FLT_HH:
   case ObjectExpression::ET_FLT_H:
   case ObjectExpression::ET_FLT:
   case ObjectExpression::ET_FLT_L:
   case ObjectExpression::ET_FLT_LL:
      return ObjectExpression::ET_FLT;

   case ObjectExpression::ET_INT_HH:
   case ObjectExpression::ET_INT_H:
   case ObjectExpression::ET_INT:
   case ObjectExpression::ET_INT_L:
   case ObjectExpression::ET_INT_LL:
      return ObjectExpression::ET_INT;

   case ObjectExpression::ET_UNS_HH:
   case ObjectExpression::ET_UNS_H:
   case ObjectExpression::ET_UNS:
   case ObjectExpression::ET_UNS_L:
   case ObjectExpression::ET_UNS_LL:
      return ObjectExpression::ET_UNS;

   case ObjectExpression::ET_OCS:
   case ObjectExpression::ET_ARR:
   case ObjectExpression::ET_MAP:
      return et;
   }

   return et;
}

//
// MakeObject
//
static ObjectExpression::Reference MakeObject(VariableType *type, SourcePosition const &pos)
{
   ObjectExpression::Vector    elems;
   VariableType::VecStr const &names = type->getNames();
   VariableType::Vector const &types = type->getTypes();

   VariableType::Vector::const_iterator itr, end = types.end();

   switch(GetET(type))
   {
   case ObjectExpression::ET_FIX_HH: return ObjectExpression::CreateValueFIX_HH(0, pos);
   case ObjectExpression::ET_FIX_H:  return ObjectExpression::CreateValueFIX_H (0, pos);
   case ObjectExpression::ET_FIX:    return ObjectExpression::CreateValueFIX   (0, pos);
   case ObjectExpression::ET_FIX_L:  return ObjectExpression::CreateValueFIX_L (0, pos);
   case ObjectExpression::ET_FIX_LL: return ObjectExpression::CreateValueFIX_LL(0, pos);

   case ObjectExpression::ET_FLT_HH: return ObjectExpression::CreateValueFLT_HH(0, pos);
   case ObjectExpression::ET_FLT_H:  return ObjectExpression::CreateValueFLT_H (0, pos);
   case ObjectExpression::ET_FLT:    return ObjectExpression::CreateValueFLT   (0, pos);
   case ObjectExpression::ET_FLT_L:  return ObjectExpression::CreateValueFLT_L (0, pos);
   case ObjectExpression::ET_FLT_LL: return ObjectExpression::CreateValueFLT_LL(0, pos);

   case ObjectExpression::ET_INT_HH: return ObjectExpression::CreateValueINT_HH(0, pos);
   case ObjectExpression::ET_INT_H:  return ObjectExpression::CreateValueINT_H (0, pos);
   case ObjectExpression::ET_INT:    return ObjectExpression::CreateValueINT   (0, pos);
   case ObjectExpression::ET_INT_L:  return ObjectExpression::CreateValueINT_L (0, pos);
   case ObjectExpression::ET_INT_LL: return ObjectExpression::CreateValueINT_LL(0, pos);

   case ObjectExpression::ET_UNS_HH: return ObjectExpression::CreateValueUNS_HH(0, pos);
   case ObjectExpression::ET_UNS_H:  return ObjectExpression::CreateValueUNS_H (0, pos);
   case ObjectExpression::ET_UNS:    return ObjectExpression::CreateValueUNS   (0, pos);
   case ObjectExpression::ET_UNS_L:  return ObjectExpression::CreateValueUNS_L (0, pos);
   case ObjectExpression::ET_UNS_LL: return ObjectExpression::CreateValueUNS_LL(0, pos);

   case ObjectExpression::ET_OCS:
      return ObjectExpression::CreateValueOCS(ObjectCodeSet(), pos);

   case ObjectExpression::ET_ARR:
      if(type->getBasicType() == VariableType::BT_ARR)
      {
         for(bigsint i = type->getWidth(); i--;)
            elems.push_back(MakeObject(type->getReturn(), pos));
      }
      else
      {
         for(itr = types.begin(); itr != end; ++itr)
            elems.push_back(MakeObject(*itr, pos));
      }

      return ObjectExpression::CreateValueARR(elems, pos);

   case ObjectExpression::ET_MAP:
      for(itr = types.begin(); itr != end; ++itr)
         elems.push_back(MakeObject(*itr, pos));

      return ObjectExpression::CreateValueMAP(elems, names, pos);
   }

   Error_P("unknown BT");
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_object_cast
//
ObjectExpression::Reference SourceExpression::make_object_cast(ObjectExpression *src,
   VariableType *dstType, VariableType *srcType, SourcePosition const &pos)
{
   ObjectExpression::Reference obj(src);

   // Same type, so no cast.
   if(dstType->getUnqualified() == srcType->getUnqualified())
      return obj;

   VariableType::BasicType dstBT = dstType->getBasicType();
   VariableType::BasicType srcBT = srcType->getBasicType();

   // Cast to bool is always the same.
   if(VariableType::IsTypeBoolean(dstBT))
   {
      obj = ObjectExpression::create_branch_not(obj, pos);
      obj = ObjectExpression::create_branch_not(obj, pos);
      return obj;
   }

   // string->array
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_ARR)
   {
      std::string srcStr = src->resolveString();

      ObjectExpression::Vector srcVec(srcStr.size());

      std::string::iterator srcStrItr = srcStr.begin(), srcStrEnd = srcStr.end();
      ObjectExpression::Vector::iterator srcVecItr = srcVec.begin();
      while(srcStrItr != srcStrEnd)
         *srcVecItr++ = ObjectExpression::CreateValueINT(*srcStrItr++, pos);

      return ObjectExpression::CreateValueARR(srcVec, pos);
   }

   // String-to-far*.
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_PTR &&
      dstType->getReturn()->getStoreType() == STORE_FAR)
   {
      BAD_CAST();
   }

   // String-to-strptr.
   // Note that the compile-time representation differs from the run-time one.
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_PTR &&
      dstType->getReturn()->getStoreType() == STORE_STRING)
   {
      auto objPtr = ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS_L, obj, pos);
      auto obj32  = ObjectExpression::CreateValueUNS_L(32, pos);
      return ObjectExpression::create_binary_lsh(objPtr, obj32, pos);
   }

   // Strptr-to-string.
   if(dstBT == VariableType::BT_STR && srcBT == VariableType::BT_PTR &&
      srcType->getReturn()->getStoreType() == STORE_STRING)
   {
      auto obj32  = ObjectExpression::CreateValueUNS_L(32, pos);
      auto objPtr = ObjectExpression::create_binary_rsh(obj, obj32, pos);
      return ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS, objPtr, pos);
   }

   // Pointer-to-pointer casts are special if changing storage.
   if(srcBT == VariableType::BT_PTR && dstBT == VariableType::BT_PTR)
   {
      // OK, so for the purposes of this block there are three types of pointers.
      // A "near" pointer is basically an old-style, storage-specific pointer.
      // A "far" pointer is a STORE_FAR pointer. And a "string" pointer is a
      // STORE_STRING pointer (surprisingly enough).

      StoreType dstST = dstType->getReturn()->getStoreType();
      StoreType srcST = srcType->getReturn()->getStoreType();

      bool dstNear = dstType->getSize(pos) == 1;
      bool srcNear = srcType->getSize(pos) == 1;

      // Nothing to change if they're the same storage.
      // Or if they're both "near" pointers.
      if(srcST == dstST || (srcNear && dstNear))
         return obj;

      auto obj32 = ObjectExpression::CreateValueUNS_L(32, pos);
      auto objMask = ObjectExpression::CreateValueUNS_L(0xFFFFFFFF, pos);

      // Cast from far to near is pure truncation!
      // HACK! Should only need the cast.
      if(srcST == STORE_FAR && dstNear)
      {
         auto objMask = ObjectExpression::CreateValueUNS_L(0xFFFFFFFF, pos);
         auto objPtr = ObjectExpression::create_binary_and(obj, objMask, pos);
         return ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS, objPtr, pos);
      }

      // Casting from near to far.
      if(srcNear && dstST == STORE_FAR)
      {
         biguint ptrBase;

         switch(srcST)
         {
         case STORE_STATIC:
            return ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS, obj, pos);

         case STORE_WORLDARRAY:  ptrBase = 0x80000000; break;
         case STORE_GLOBALARRAY: ptrBase = 0x80010000; break;
         default: BAD_CAST();
         }

         auto objPtr = ObjectExpression::CreateValueUNS_L(ptrBase, pos);

         auto objSA = ObjectExpression::CreateValueSymbol(srcType->getReturn()->getStoreArea(), pos);
         objSA = ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS_L, objSA, pos);

         objPtr = ObjectExpression::create_binary_ior(objPtr, objSA, pos);
         objPtr = ObjectExpression::create_binary_lsh(objPtr, obj32, pos);
         objPtr = ObjectExpression::create_binary_ior(objPtr, obj, pos);

         // But here's the thing. If it's null, it needs to stay null.
         auto objOrig = ObjectExpression::CreateValueCast(ObjectExpression::ET_UNS_L, obj, pos);
         auto objNull = ObjectExpression::create_branch_not(obj, pos);

         return ObjectExpression::create_branch_if(objNull, objOrig, objPtr, pos);
      }

      // Invalid at compile-time: near->string, far->string, string->near, string->far
      // This is because strings can't be tagged inside far pointers.
      BAD_CAST();
   }

   ObjectExpression::ExpressionType dstET = GetET(dstType);
   ObjectExpression::ExpressionType srcET = GetET(srcType);

   switch(srcET)
   {
   case ObjectExpression::ET_OCS:
      BAD_CAST();

   case ObjectExpression::ET_ARR:
      if(srcBT != VariableType::BT_BLOCK)
         BAD_CAST();
   {
      VariableType::Vector const &srcTypes = srcType->getTypes();
      ObjectExpression::Vector elems;
      obj->expandOnce(&elems);

      if(dstBT == VariableType::BT_ARR)
      {
         VariableType::Reference dstTypes = dstType->getReturn();
         biguint                 dstWidth = dstType->getWidth();

         elems.resize(dstWidth);
         for(biguint i = 0; i < dstWidth; ++i)
         {
            if(elems[i])
               elems[i] = make_object_cast(elems[i], dstTypes, srcTypes[i], pos);
            else
               elems[i] = MakeObject(dstTypes, pos);
         }

         obj = ObjectExpression::CreateValueARR(elems, pos);
      }
      else if(dstBT == VariableType::BT_STRUCT ||
              dstBT == VariableType::BT_BLOCK)
      {
         VariableType::Vector const &dstTypes = dstType->getTypes();
         biguint                     dstWidth = dstTypes.size();

         elems.resize(dstWidth);
         for(biguint i = 0; i < dstWidth; ++i)
         {
            if(elems[i])
               elems[i] = make_object_cast(elems[i], dstTypes[i], srcTypes[i], pos);
            else
               elems[i] = MakeObject(dstTypes[i], pos);
         }

         if(dstBT == VariableType::BT_STRUCT)
            obj = ObjectExpression::CreateValueMAP(elems, dstType->getNames(), pos);
         else
            obj = ObjectExpression::CreateValueARR(elems, pos);
      }
      else if(!elems.empty())
         obj = make_object_cast(elems.back(), dstType, srcTypes.back(), pos);
      else
         obj = MakeObject(dstType, pos);

      return obj;
   }

   case ObjectExpression::ET_MAP:
      BAD_CAST();

   default:
      return ObjectExpression::CreateValueCast(dstET, src, pos);
   }

   BAD_CAST();
}

//
// SourceExpression::make_objects_memcpy_cast
//
void SourceExpression::make_objects_memcpy_cast
(ObjectVector *objects, VariableData *dst, VariableData *src,
 VariableType *dstType, VariableType *srcType, SourceContext *context,
 SourcePosition const &pos)
{
   // If no cast, this just becomes a memcpy.
   if (dstType->getUnqualified() == srcType->getUnqualified())
   {
      make_objects_memcpy_post(objects, dst, src, srcType, context, pos);

      return;
   }

   VariableType::BasicType dstBT = dstType->getBasicType();
   VariableType::BasicType srcBT = srcType->getBasicType();

   // If casting to void, just memcpy to void.
   if (dstBT == VariableType::BT_VOID)
   {
      make_objects_memcpy_void(objects, src, pos);

      // If there's an offset temp, it needs to be dealt with.
      if (dst->offsetTemp)
         make_objects_memcpy_void(objects, dst->offsetTemp, pos);

      return;
   }

   // All casting is stack-ops.
   VariableData::Pointer tmp = VariableData::create_stack(src->size);

   make_objects_memcpy_prep(objects, tmp, src, pos);
   make_objects_memcpy_post(objects, tmp, src, srcType, context, pos);

   ObjectExpression::ExpressionType dstET = GetET(dstType);
   ObjectExpression::ExpressionType srcET = GetET(srcType);

   biguint dstSize = dst->size, srcSize = src->size;

   // Pointer information.
   StoreType dstST, srcST;
   bool dstNear, srcNear;

   // Strptr information.
   ObjectExpression::Pointer tmpStr, tmpOff;

   std::string label, labelEnd;

   // String-to-far*.
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_PTR &&
      dstType->getReturn()->getStoreType() == STORE_FAR)
   {
      tmpStr = context->getTempVar(0);

      objects->addToken(OCODE_SET_TEMP, tmpStr);

      objects->addToken(OCODE_GET_TEMP, tmpStr);
      objects->addToken(OCODE_GET_IMM,  objects->getValue(0x3FFFFFFF));
      objects->addToken(OCODE_AND_STK_U);
      objects->addToken(OCODE_GET_IMM,  objects->getValue(0x40000000));
      objects->addToken(OCODE_IOR_STK_U);

      objects->addToken(OCODE_GET_TEMP, tmpStr);
      objects->addToken(OCODE_GET_IMM,  objects->getValue(0xC0000000));
      objects->addToken(OCODE_AND_STK_U);

      goto cast_done;
   }

   // Far*-to-string.
   if(dstBT == VariableType::BT_STR && srcBT == VariableType::BT_PTR &&
      srcType->getReturn()->getStoreType() == STORE_FAR)
   {
      objects->addToken(OCODE_GET_IMM,  objects->getValue(0xC0000000));
      objects->addToken(OCODE_AND_STK_U);

      objects->addToken(OCODE_STK_SWAP);

      objects->addToken(OCODE_GET_IMM,  objects->getValue(0x3FFFFFFF));
      objects->addToken(OCODE_AND_STK_U);

      objects->addToken(OCODE_IOR_STK_U);

      goto cast_done;
   }

   // Pointer to pointer casts are special.
   if(srcBT == VariableType::BT_PTR && dstBT == VariableType::BT_PTR)
   {
      // An overview of the types of pointers given above.

      dstST = dstType->getReturn()->getStoreType();
      srcST = srcType->getReturn()->getStoreType();

      dstNear = dstType->getSize(pos) == 1;
      srcNear = srcType->getSize(pos) == 1;

      // Special handling for auto*->static*.
      if(srcType->getReturn()->getStoreType() == STORE_AUTO &&
         dstType->getReturn()->getStoreType() == STORE_STATIC)
      {
         objects->addToken(OCODE_GET_AUTPTR);
         objects->addToken(OCODE_ADD_STK_U);
         goto cast_done;
      }

      // Nothing to change if they're the same storage.
      // Or if they're both "near" pointers. (The above notwithstanding.)
      if(srcST == dstST || (srcNear && dstNear))
         goto cast_done;

      // Cast from far to near is pure truncation!
      if(srcST == STORE_FAR && dstNear)
      {
         objects->addToken(OCODE_STK_SWAP);
         objects->addToken(OCODE_STK_DROP);
         goto cast_done;
      }

      // Near to far.
      if(srcNear && dstST == STORE_FAR)
      {
         switch(srcST)
         {
         case STORE_AUTO:
            objects->addToken(OCODE_GET_AUTPTR);
            objects->addToken(OCODE_ADD_STK_U);
         case STORE_STATIC:
            objects->addTokenPushZero();
            objects->addToken(OCODE_STK_SWAP);
            goto cast_done;

         case STORE_WORLDARRAY:
            tmpStr = objects->getValue(0x80000000);

         gblarr_to_far:
            label = context->makeLabel();
            labelEnd = label + "_end";

            tmpOff = objects->getValue(srcType->getReturn()->getStoreArea());
            tmpOff = ObjectExpression::create_binary_ior(tmpStr, tmpOff, pos);

            // if(stk) PUSH(storeArea) else PUSH(0); SWAP();
            objects->addToken(OCODE_STK_COPY);
            objects->addToken(OCODE_JMP_TRU, objects->getValue(label));
            objects->addTokenPushZero();
            objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));
            objects->addLabel(label);
            objects->addToken(OCODE_GET_IMM, tmpOff);
            objects->addLabel(labelEnd);
            objects->addToken(OCODE_STK_SWAP);

            goto cast_done;

         case STORE_GLOBALARRAY:
            tmpStr = objects->getValue(0x80010000);
            goto gblarr_to_far;

         default:
            BAD_CAST();
         }
      }

      // String to far.
      if(srcST == STORE_STRING && dstST == STORE_FAR)
      {
         label = context->makeLabel();

         tmpStr = context->getTempVar(0);
         tmpOff = context->getTempVar(1);

         objects->addToken(OCODE_SET_TEMP,   tmpOff);
         objects->addToken(OCODE_SET_TEMP,   tmpStr);

         // Null pointers must stay null.
         objects->addToken(OCODE_GET_TEMP,   tmpStr);
         objects->addToken(OCODE_GET_TEMP,   tmpOff);
         objects->addToken(OCODE_IOR_STK_U);
         objects->addToken(OCODE_JMP_NIL,    objects->getValue(label));

         // off &= ~0xC0000000; // Unneeded. If those bits set, undefined.
       //objects->addToken(OCODE_GET_IMM,    objects->getValue(0x3FFFFFFF);
       //objects->addToken(OCODE_AND_TEMP_U, tmpOff);
         // off |= str & 0xC0000000;
         objects->addToken(OCODE_GET_TEMP,   tmpStr);
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0xC0000000));
         objects->addToken(OCODE_AND_STK_U);
         objects->addToken(OCODE_IOR_TEMP_U, tmpOff);
         // str &= ~0xC0000000;
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0x3FFFFFFF));
         objects->addToken(OCODE_AND_TEMP_U, tmpStr);
         // str |= 0x40000000;
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0x40000000));
         objects->addToken(OCODE_IOR_TEMP_U, tmpStr);

         objects->addLabel(label);
         objects->addToken(OCODE_GET_TEMP,   tmpStr);
         objects->addToken(OCODE_GET_TEMP,   tmpOff);

         goto cast_done;
      }

      // Far to string.
      if(srcST == STORE_FAR && dstST == STORE_STRING)
      {
         tmpStr = context->getTempVar(0);
         tmpOff = context->getTempVar(1);

         objects->addToken(OCODE_SET_TEMP,   tmpOff);
         objects->addToken(OCODE_SET_TEMP,   tmpStr);

         // Null pointers must stay null.
         // But in this case, this is handled implicitly.

         // str &= ~0xC0000000;
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0x3FFFFFFF));
         objects->addToken(OCODE_AND_TEMP_U, tmpStr);
         // str |= off & 0xC0000000;
         objects->addToken(OCODE_GET_TEMP,   tmpOff);
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0xC0000000));
         objects->addToken(OCODE_AND_STK_U);
         objects->addToken(OCODE_IOR_TEMP_U, tmpStr);
         // off &= ~0xC0000000;
         objects->addToken(OCODE_GET_IMM,    objects->getValue(0x3FFFFFFF));
         objects->addToken(OCODE_AND_TEMP_U, tmpOff);

         objects->addToken(OCODE_GET_TEMP,   tmpStr);
         objects->addToken(OCODE_GET_TEMP,   tmpOff);

         goto cast_done;
      }

      // Anything else has to be an error.
      BAD_CAST();
   }

   // Casting to boolean is always the same.
   if(dstBT == VariableType::BT_BIT_HRD)
   {
      if(srcSize == 1)
      {
         objects->addToken(OCODE_NOT_STK_I);
         objects->addToken(OCODE_NOT_STK_I);
      }
      else if(srcSize)
         while(--srcSize) objects->addToken(OCODE_LOGIOR_STK_I);
      else
         objects->addTokenPushZero();

      goto cast_done;
   }

   // Same for soft boolean.
   if(dstBT == VariableType::BT_BIT_SFT)
   {
      if(srcSize)
         while(--srcSize) objects->addToken(OCODE_IOR_STK_I);
      else
         objects->addTokenPushZero();

      goto cast_done;
   }

   // If casting from soft boolean, booleanize it first.
   if(srcBT == VariableType::BT_BIT_SFT)
   {
      objects->addToken(OCODE_NOT_STK_I);
      objects->addToken(OCODE_NOT_STK_I);
   }

   switch(GetSimpleET(srcET))
   {
   case ObjectExpression::ET_FIX:
      ExtendSign(objects, dstSize, srcSize, context);
      for(; srcSize > dstSize; --srcSize) objects->addToken(OCODE_STK_DROP);

      switch(GetSimpleET(dstET))
      {
      case ObjectExpression::ET_FIX:
         goto cast_done;

      case ObjectExpression::ET_INT:
      case ObjectExpression::ET_UNS:
         objects->addToken(OCODE_CAST_STK_X2I);
         goto cast_done;

      default: BAD_CAST();
      }

   case ObjectExpression::ET_INT:
      ExtendSign(objects, dstSize, srcSize, context);
   case ObjectExpression::ET_UNS:
      for(; srcSize < dstSize; ++srcSize) objects->addTokenPushZero();
      for(; srcSize > dstSize; --srcSize) objects->addToken(OCODE_STK_DROP);

      switch(GetSimpleET(dstET))
      {
      case ObjectExpression::ET_FIX:
         objects->addToken(OCODE_CAST_STK_I2X);
      case ObjectExpression::ET_INT:
      case ObjectExpression::ET_UNS:
         goto cast_done;

      default: BAD_CAST();
      }

   default: BAD_CAST();
   }

cast_done:
   make_objects_memcpy_post(objects, dst, tmp, dstType, context, pos);
}

// EOF

