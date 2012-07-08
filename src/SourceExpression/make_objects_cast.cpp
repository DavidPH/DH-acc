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
// Static Functions                                                           |
//

static ObjectExpression::Pointer make_object
(VariableType *type, SourcePosition const &pos)
{
   ObjectExpression::Vector    elems;
   VariableType::VecStr const &names = type->getNames();
   VariableType::Vector const &types = type->getTypes();

   VariableType::Vector::const_iterator iter;

   switch (type->getBasicType())
   {
   case VariableType::BT_VOID:
      ERROR_P("make_object BT_VOID");

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_BIT_SFT:
   case VariableType::BT_CHR:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
      return ObjectExpression::create_value_int(0, pos);

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
      return ObjectExpression::create_value_float(0, pos);

   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
      return ObjectExpression::create_value_int(0, pos);

   case VariableType::BT_ARR:
      for (bigsint i = type->getWidth(); i--;)
         elems.push_back(make_object(type->getReturn(), pos));
      return ObjectExpression::create_value_array(elems, pos);

   case VariableType::BT_PTR:
   case VariableType::BT_PTR_NUL:
      return ObjectExpression::create_value_int(0, pos);

   case VariableType::BT_ENUM:
      return ObjectExpression::create_value_int(0, pos);

   case VariableType::BT_STRUCT:
      for (iter = types.begin(); iter != types.end(); ++iter)
         elems.push_back(make_object(*iter, pos));
      return ObjectExpression::create_value_struct(elems, names, pos);

   case VariableType::BT_UNION:
      ERROR_P("make_object BT_UNION");

   case VariableType::BT_BLOCK:
      for (iter = types.begin(); iter != types.end(); ++iter)
         elems.push_back(make_object(*iter, pos));
      return ObjectExpression::create_value_array(elems, pos);

   case VariableType::BT_ASMFUNC:
      ERROR_P("make_object BT_ASMFUNC");

   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SNAM:
   case VariableType::BT_SNUM:
      return ObjectExpression::create_value_int(0, pos);
   }

   ERROR_P("unknown BT");
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_object_cast
//
ObjectExpression::Pointer SourceExpression::make_object_cast
(ObjectExpression *src, VariableType *dstType, VariableType *srcType,
 SourcePosition const &pos)
{
   // Same type, so no cast.
   if (dstType->getUnqualified() == srcType->getUnqualified())
      return src;

   VariableType::BasicType dstBT = dstType->getBasicType();
   VariableType::BasicType srcBT = srcType->getBasicType();

   ObjectExpression::Pointer obj = src;

   switch (srcBT)
   {
   case VariableType::BT_VOID:
   case VariableType::BT_ARR:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_ASMFUNC:
   case_src_bad:
      ERROR_P("bad cast: %s to %s", make_string(srcType).c_str(),
              make_string(dstType).c_str());

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_CHR:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
   case VariableType::BT_PTR_NUL:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SNAM:
   case VariableType::BT_SNUM:
   case_src_int:
      switch (dstBT)
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
      case VariableType::BT_ASMFUNC:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
         obj = ObjectExpression::create_branch_not(obj, pos);
         obj = ObjectExpression::create_branch_not(obj, pos);
         break;

      case VariableType::BT_BIT_SFT:
      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
         obj = ObjectExpression::create_cast_int_to_float(obj, pos);
         break;
      }
      break;

   case VariableType::BT_BIT_SFT:
      switch (dstBT)
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         obj = ObjectExpression::create_branch_not(obj, pos);
         obj = ObjectExpression::create_branch_not(obj, pos);
         break;

      case VariableType::BT_BIT_SFT:
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
         obj = ObjectExpression::create_branch_not(obj, pos);
         obj = ObjectExpression::create_branch_not(obj, pos);
         obj = ObjectExpression::create_cast_int_to_float(obj, pos);
         break;
      }
      break;

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
      switch (dstBT)
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
         goto case_src_bad;

      case VariableType::BT_BIT_SFT:
         obj = ObjectExpression::create_branch_not(obj, pos);
         obj = ObjectExpression::create_branch_not(obj, pos);
      case VariableType::BT_BIT_HRD:
      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         obj = ObjectExpression::create_cast_float_to_int(obj, pos);
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
         break;
      }
      break;

   case VariableType::BT_PTR:
      if (dstBT != VariableType::BT_PTR)
         goto case_src_int;

      if (srcType->getReturn()->getStoreType() != STORE_AUTO ||
          dstType->getReturn()->getStoreType() != STORE_STATIC)
         goto case_src_int;

      // Special handling for auto*->static*.
      ERROR_P("bad compile-time cast: %s to %s", make_string(srcType).c_str(),
         make_string(dstType).c_str());

   case VariableType::BT_BLOCK:
   {
      VariableType::Vector const &srcTypes = srcType->getTypes();
      ObjectExpression::Vector elems;
      obj->expandOnce(&elems);

      if (dstBT == VariableType::BT_ARR)
      {
         VariableType::Reference dstTypes = dstType->getReturn();
         bigsint                 dstWidth = dstType->getWidth();

         elems.resize(dstWidth);
         for (bigsint i = 0; i < dstWidth; ++i)
         {
            if (elems[i])
               elems[i] = make_object_cast(
                  elems[i], dstTypes, srcTypes[i], pos);
            else
               elems[i] = make_object(dstTypes, pos);
         }

         obj = ObjectExpression::create_value_array(elems, pos);
      }
      else if (dstBT == VariableType::BT_STRUCT ||
               dstBT == VariableType::BT_BLOCK)
      {
         VariableType::Vector const &dstTypes = dstType->getTypes();

         elems.resize(dstTypes.size());
         for (size_t i = 0; i < dstTypes.size(); ++i)
         {
            if (elems[i])
               elems[i] = make_object_cast(
                  elems[i], dstTypes[i], srcTypes[i], pos);
            else
               elems[i] = make_object(dstTypes[i], pos);
         }

         if (dstBT == VariableType::BT_STRUCT)
            obj = ObjectExpression::create_value_struct
               (elems, dstType->getNames(), pos);
      }
      else if (!elems.empty())
         obj = make_object_cast(elems.back(), dstType, srcTypes.back(), pos);
      else
         obj = make_object(dstType, pos);
   }
      break;
   }

   return obj;
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

   switch (srcBT)
   {
   case VariableType::BT_VOID:
   case VariableType::BT_ARR:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_BLOCK:
   case VariableType::BT_ASMFUNC:
   case_src_bad:
      ERROR_P("bad cast: %s to %s", make_string(srcType).c_str(),
              make_string(dstType).c_str());

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_CHR:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
   case VariableType::BT_PTR_NUL:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SNAM:
   case VariableType::BT_SNUM:
   case_src_int:
      switch (dstBT)
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
      case VariableType::BT_ASMFUNC:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
         objects->addToken(OCODE_NOT_STK_I);
         objects->addToken(OCODE_NOT_STK_I);
         break;

      case VariableType::BT_BIT_SFT:
      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
         objects->addToken(OCODE_CAST_STK_I2X);
         break;

      case VariableType::BT_INT_LL:
         if (VariableType::is_bt_unsigned(srcBT))
         {
            objects->addTokenPushZero();
            break;
         }

      {
         // Must sign-extend. Which means... BRANCHING! AAAAAAAA!
         std::string label = context->makeLabel();
         std::string labelPos = label + "_pos";
         std::string labelEnd = label + "_end";
         objects->addToken(OCODE_STK_COPY);
         objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
         objects->addToken(OCODE_AND_STK_I);
         objects->addToken(OCODE_JMP_NIL, objects->getValue(labelPos));
         objects->addToken(OCODE_GET_IMM, objects->getValue(0xFFFFFFFF));
         objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));
         objects->addLabel(labelPos);
         objects->addToken(OCODE_GET_IMM, objects->getValue(0x00000000));
         objects->addLabel(labelEnd);
      }
         break;

      case VariableType::BT_UNS_LL:
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_BIT_SFT:
      switch (dstBT)
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
      case VariableType::BT_ASMFUNC:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         objects->addToken(OCODE_NOT_STK_I);
         objects->addToken(OCODE_NOT_STK_I);
         break;

      case VariableType::BT_BIT_SFT:
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
         objects->addToken(OCODE_NOT_STK_X);
         objects->addToken(OCODE_NOT_STK_X);
         objects->addToken(OCODE_CAST_STK_I2X);
         break;

      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_LL:
         objects->addToken(OCODE_NOT_STK_I);
         objects->addToken(OCODE_NOT_STK_I);
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
      case VariableType::BT_ASMFUNC:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
         objects->addToken(OCODE_NOT_STK_I);
         objects->addToken(OCODE_NOT_STK_I);
         break;

      case VariableType::BT_BIT_SFT:
      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
         break;

      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         objects->addToken(OCODE_CAST_STK_X2I);
         break;

      case VariableType::BT_INT_LL:
      {
         // Must sign-extend. Which means... BRANCHING! AAAAAAAA!
         std::string label = context->makeLabel();
         std::string labelPos = label + "_pos";
         std::string labelEnd = label + "_end";
         objects->addToken(OCODE_STK_COPY);
         objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
         objects->addToken(OCODE_AND_STK_I);
         objects->addToken(OCODE_JMP_NIL, objects->getValue(labelPos));
         objects->addToken(OCODE_GET_IMM, objects->getValue(0xFFFFFFFF));
         objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));
         objects->addLabel(labelPos);
         objects->addToken(OCODE_GET_IMM, objects->getValue(0x00000000));
         objects->addLabel(labelEnd);
      }
         break;

      case VariableType::BT_UNS_LL:
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_LL:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_VOID:
      case VariableType::BT_ARR:
      case VariableType::BT_FLT_HH:
      case VariableType::BT_FLT_H:
      case VariableType::BT_FLT:
      case VariableType::BT_FLT_L:
      case VariableType::BT_FLT_LL:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_BLOCK:
      case VariableType::BT_ASMFUNC:
         goto case_src_bad;

      case VariableType::BT_BIT_HRD:
         objects->addToken(OCODE_LOGIOR_STK_I);
         break;

      case VariableType::BT_BIT_SFT:
         objects->addToken(OCODE_IOR_STK_I);
         break;

      case VariableType::BT_CHR:
      case VariableType::BT_INT_HH:
      case VariableType::BT_INT_H:
      case VariableType::BT_INT:
      case VariableType::BT_INT_L:
      case VariableType::BT_UNS_HH:
      case VariableType::BT_UNS_H:
      case VariableType::BT_UNS:
      case VariableType::BT_UNS_L:
      case VariableType::BT_LABEL:
      case VariableType::BT_STR:
      case VariableType::BT_PTR:
      case VariableType::BT_PTR_NUL:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_SNAM:
      case VariableType::BT_SNUM:
         objects->addToken(OCODE_STK_DROP);
         break;

      case VariableType::BT_FIX_HH:
      case VariableType::BT_FIX_H:
      case VariableType::BT_FIX:
      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
         // What about the high byte, you ask? If the high byte is meaningful,
         // we've overflowed the fixed anyway. Therefore: Undefined behavior.
         objects->addToken(OCODE_STK_DROP);
         objects->addToken(OCODE_CAST_STK_I2X);
         break;

      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_LL:
         break;
      }
      break;

   case VariableType::BT_PTR:
      if (dstBT != VariableType::BT_PTR)
         goto case_src_int;

      if (srcType->getReturn()->getStoreType() != STORE_AUTO ||
          dstType->getReturn()->getStoreType() != STORE_STATIC)
         goto case_src_int;

      // Special handling for auto*->static*.
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_ADD_STK_U);
      break;
   }

   make_objects_memcpy_post(objects, dst, tmp, dstType, context, pos);
}

// EOF

