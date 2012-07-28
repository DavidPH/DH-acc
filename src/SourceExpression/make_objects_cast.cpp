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

#define BAD_CAST() ERROR_P("bad cast: %s to %s", \
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
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
      return ObjectExpression::ET_INT;

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
      return ObjectExpression::ET_MAP;

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
      return ObjectExpression::ET_FIX;

   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
      return ObjectExpression::ET_FLT;

   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_ARR:
      return ObjectExpression::ET_ARR;

   case VariableType::BT_PTR:
   case VariableType::BT_PTR_NUL:
      return ObjectExpression::ET_UNS;

   case VariableType::BT_ENUM:
      return ObjectExpression::ET_INT;

   case VariableType::BT_STRUCT:
      return ObjectExpression::ET_MAP;

   case VariableType::BT_UNION:
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
   case ObjectExpression::ET_FIX: return ObjectExpression::create_value_fix(0, pos);
   case ObjectExpression::ET_FLT: return ObjectExpression::create_value_flt(0, pos);
   case ObjectExpression::ET_INT: return ObjectExpression::create_value_int(0, pos);
   case ObjectExpression::ET_UNS: return ObjectExpression::create_value_uns(0, pos);
   case ObjectExpression::ET_OCS:
      return ObjectExpression::create_value_ocs(ObjectCodeSet(), pos);

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

      return ObjectExpression::create_value_arr(elems, pos);

   case ObjectExpression::ET_MAP:
      for(itr = types.begin(); itr != end; ++itr)
         elems.push_back(MakeObject(*itr, pos));

      return ObjectExpression::create_value_map(elems, names, pos);
   }

   ERROR_P("unknown BT");
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
   if(VariableType::is_bt_bit(dstBT))
   {
      obj = ObjectExpression::create_branch_not(obj, pos);
      obj = ObjectExpression::create_branch_not(obj, pos);
      return obj;
   }

   // String-to-far*.
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_PTR &&
      dstType->getReturn()->getStoreType() == STORE_NONE)
   {
      BAD_CAST();
   }

   // String-to-strptr.
   // Note that the compile-time representation differs from the run-time one.
   if(srcBT == VariableType::BT_STR && dstBT == VariableType::BT_PTR &&
      dstType->getReturn()->getStoreType() == STORE_STRING)
   {
      ObjectExpression::Reference obj32 = ObjectExpression::create_value_uns(32, pos);
      return ObjectExpression::create_binary_lsh(obj, obj32, pos);
   }

   // Strptr-to-string.
   if(dstBT == VariableType::BT_STR && srcBT == VariableType::BT_PTR &&
      srcType->getReturn()->getStoreType() == STORE_STRING)
   {
      ObjectExpression::Reference obj32 = ObjectExpression::create_value_uns(32, pos);
      return ObjectExpression::create_binary_rsh(obj, obj32, pos);
   }

   // Pointer-to-pointer casts are special if changing storage.
   if(srcBT == VariableType::BT_PTR && dstBT == VariableType::BT_PTR)
   {
      // OK, so for the purposes of this block there are three types of pointers.
      // A "near" pointer is basically an old-style, storage-specific pointer.
      // A "far" pointer is a STORE_NONE pointer. And a "string" pointer is a
      // STORE_STRING pointer (surprisingly enough).

      StoreType dstST = dstType->getReturn()->getStoreType();
      StoreType srcST = srcType->getReturn()->getStoreType();

      bool dstNear = dstType->getSize(pos) == 1;
      bool srcNear = srcType->getSize(pos) == 1;

      // Nothing to change if they're the same storage.
      // Or if they're both "near" pointers.
      if(srcST == dstST || (srcNear && dstNear))
         return obj;

      ObjectExpression::Reference obj32 = ObjectExpression::create_value_uns(32, pos);
      ObjectExpression::Reference objMask = ObjectExpression::create_value_uns(0xFFFFFFFF, pos);

      // Cast from far to near is pure truncation!
      if(srcST == STORE_NONE && dstNear)
         return ObjectExpression::create_binary_and(obj, objMask, pos);

      // Casting from near to far.
      if(srcNear && dstST == STORE_NONE)
      {
         biguint ptrBase;

         switch(srcST)
         {
         case STORE_STATIC: return obj;
         case STORE_WORLDARRAY:  ptrBase = 0x80000000; break;
         case STORE_GLOBALARRAY: ptrBase = 0x80010000; break;
         default: BAD_CAST();
         }

         ObjectExpression::Reference objPtr = ObjectExpression::
            create_value_uns(ptrBase, pos);

         ObjectExpression::Reference objSA = ObjectExpression::
            create_value_symbol(srcType->getReturn()->getStoreArea(), pos);

         objPtr = ObjectExpression::create_binary_ior(objPtr, objSA, pos);
         objPtr = ObjectExpression::create_binary_lsh(objPtr, obj32, pos);
         objPtr = ObjectExpression::create_binary_ior(objPtr, obj, pos);

         // But here's the thing. If it's null, it needs to stay null.
         ObjectExpression::Reference objNull = ObjectExpression::create_branch_not(obj, pos);

         return ObjectExpression::create_branch_if(objNull, obj, objPtr, pos);
      }

      // Invalid at compile-time: near->string, far->string, string->near, string->far
      // This is because strings can't be tagged inside far pointers.
      BAD_CAST();
   }

   ObjectExpression::ExpressionType dstET = GetET(dstType);
   ObjectExpression::ExpressionType srcET = GetET(srcType);

   switch(srcET)
   {
   case ObjectExpression::ET_FIX:
      switch(dstET)
      {
      case ObjectExpression::ET_FIX: return obj;
      case ObjectExpression::ET_FLT: return ObjectExpression::create_cast_fix_to_flt(obj, pos);
      case ObjectExpression::ET_INT: return ObjectExpression::create_cast_fix_to_int(obj, pos);
      case ObjectExpression::ET_UNS: return ObjectExpression::create_cast_fix_to_uns(obj, pos);

      default: BAD_CAST();
      }

   case ObjectExpression::ET_FLT:
      switch(dstET)
      {
      case ObjectExpression::ET_FIX: return ObjectExpression::create_cast_flt_to_fix(obj, pos);
      case ObjectExpression::ET_FLT: return obj;
      case ObjectExpression::ET_INT: return ObjectExpression::create_cast_flt_to_int(obj, pos);
      case ObjectExpression::ET_UNS: return ObjectExpression::create_cast_flt_to_uns(obj, pos);

      default: BAD_CAST();
      }

   case ObjectExpression::ET_INT:
      switch(dstET)
      {
      case ObjectExpression::ET_FIX: return ObjectExpression::create_cast_int_to_fix(obj, pos);
      case ObjectExpression::ET_FLT: return ObjectExpression::create_cast_int_to_flt(obj, pos);
      case ObjectExpression::ET_INT: return obj;
      case ObjectExpression::ET_UNS: return ObjectExpression::create_cast_int_to_uns(obj, pos);

      default: BAD_CAST();
      }

   case ObjectExpression::ET_UNS:
      switch(dstET)
      {
      case ObjectExpression::ET_FIX: return ObjectExpression::create_cast_uns_to_fix(obj, pos);
      case ObjectExpression::ET_FLT: return ObjectExpression::create_cast_uns_to_flt(obj, pos);
      case ObjectExpression::ET_INT: return ObjectExpression::create_cast_uns_to_int(obj, pos);
      case ObjectExpression::ET_UNS: return obj;

      default: BAD_CAST();
      }

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

         obj = ObjectExpression::create_value_arr(elems, pos);
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
            obj = ObjectExpression::create_value_map(elems, dstType->getNames(), pos);
         else
            obj = ObjectExpression::create_value_arr(elems, pos);
      }
      else if(!elems.empty())
         obj = make_object_cast(elems.back(), dstType, srcTypes.back(), pos);
      else
         obj = MakeObject(dstType, pos);

      return obj;
   }

   case ObjectExpression::ET_MAP:
      BAD_CAST();
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
      dstType->getReturn()->getStoreType() == STORE_NONE)
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
      srcType->getReturn()->getStoreType() == STORE_NONE)
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
      if(srcST == STORE_NONE && dstNear)
      {
         objects->addToken(OCODE_STK_SWAP);
         objects->addToken(OCODE_STK_DROP);
         goto cast_done;
      }

      // Near to far.
      if(srcNear && dstST == STORE_NONE)
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
      if(srcST == STORE_STRING && dstST == STORE_NONE)
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
      if(srcST == STORE_NONE && dstST == STORE_STRING)
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

   switch(srcET)
   {
   case ObjectExpression::ET_FIX:
      ExtendSign(objects, dstSize, srcSize, context);
      for(; srcSize > dstSize; --srcSize) objects->addToken(OCODE_STK_DROP);

      switch(dstET)
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

      switch(dstET)
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

