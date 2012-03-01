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
(VariableType *type, SourcePosition const &position)
{
   ObjectExpression::Vector    elems;
   VariableType::VecStr const &names = type->getNames();
   VariableType::Vector const &types = type->getTypes();

   VariableType::Vector::const_iterator iter;

   switch (type->getBasicType())
   {
   case VariableType::BT_ARRAY:
      for (bigsint i = type->getWidth(); i--;)
         elems.push_back(make_object(type->getReturn(), position));
      return ObjectExpression::create_value_array(elems, position);

   case VariableType::BT_ASMFUNC:
      throw SourceException("make_object BT_ASMFUNC", position, __func__);

   case VariableType::BT_BLOCK:
      for (iter = types.begin(); iter != types.end(); ++iter)
         elems.push_back(make_object(*iter, position));
      return ObjectExpression::create_value_array(elems, position);

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LLONG:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_SHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      return ObjectExpression::create_value_int(0, position);

   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_REAL:
      return ObjectExpression::create_value_float(0, position);

   case VariableType::BT_STRUCT:
      for (iter = types.begin(); iter != types.end(); ++iter)
         elems.push_back(make_object(*iter, position));
      return ObjectExpression::create_value_struct(elems, names, position);

   case VariableType::BT_UNION:
      throw SourceException("make_object BT_UNION", position, __func__);

   case VariableType::BT_VOID:
      throw SourceException("make_object BT_VOID", position, __func__);
   }
   throw SourceException("stub", position, __func__);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_object_cast
//
ObjectExpression::Pointer SourceExpression::make_object_cast
(ObjectExpression *src, VariableType *dstType, VariableType *srcType,
 SourcePosition const &position)
{
   #define TYPES_STRING ("bad BT " + make_string(srcType->getBasicType()) + \
                         "->"      + make_string(dstType->getBasicType()))

   // Same type, so no cast.
   if (dstType->getUnqualified() == srcType->getUnqualified())
      return src;

   VariableType::BasicType dstBT = dstType->getBasicType();
   VariableType::BasicType srcBT = srcType->getBasicType();

   ObjectExpression::Pointer obj = src;

   switch (srcBT)
   {
   case VariableType::BT_ARRAY:
   case VariableType::BT_ASMFUNC:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_VOID:
      throw SourceException(TYPES_STRING, position, __func__);

   case VariableType::BT_BLOCK:
   {
      VariableType::Vector const &srcTypes = srcType->getTypes();
      ObjectExpression::Vector elems;
      obj->expandOnce(&elems);

      if (dstBT == VariableType::BT_ARRAY)
      {
         VariableType::Reference dstTypes = dstType->getReturn();
         bigsint                 dstWidth = dstType->getWidth();

         for (bigsint i = 0; i < dstWidth; ++i)
         {
            if (static_cast<size_t>(i) < elems.size())
               elems[i] = make_object_cast
               (elems[i], dstTypes, srcTypes[i], position);
            else
               elems.push_back(make_object(dstTypes, position));
         }
      }
      else if (dstBT == VariableType::BT_STRUCT ||
               dstBT == VariableType::BT_BLOCK)
      {
         VariableType::Vector const &dstTypes = dstType->getTypes();

         for (size_t i = 0; i < dstTypes.size(); ++i)
         {
            if (i < elems.size())
               elems[i] = make_object_cast
               (elems[i], dstTypes[i], srcTypes[i], position);
            else
               elems.push_back(make_object(dstTypes[i], position));
         }
      }
      else
         throw SourceException(TYPES_STRING, position, __func__);

      if (dstBT == VariableType::BT_STRUCT)
         obj = ObjectExpression::create_value_struct
         (elems, dstType->getNames(), position);
      else
         obj = ObjectExpression::create_value_array(elems, position);
   }
      break;

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LLONG:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_SHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      switch (dstBT)
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         break;

      case VariableType::BT_BOOLSOFT:
      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LLONG:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULLONG:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_REAL:
         obj = ObjectExpression::create_cast_int_to_float(obj, position);
         break;
      }
      break;

   case VariableType::BT_BOOLSOFT:
      switch (dstBT)
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LLONG:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULLONG:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         break;

      case VariableType::BT_BOOLSOFT:
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_REAL:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_cast_int_to_float(obj, position);
         break;
      }
      break;

   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_REAL:
      switch (dstBT)
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
      case VariableType::BT_BOOLSOFT:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_REAL:
         break;

      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LLONG:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULLONG:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         obj = ObjectExpression::create_cast_float_to_int(obj, position);
         break;
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
 SourcePosition const &position)
{
   // Don't want to slog through all that if not throwing, but also want to
   // avoid code duplication.
   #define TYPES_STRING ("bad BT " + make_string(srcType->getBasicType()) + \
                         "->"      + make_string(dstType->getBasicType()))

   // If no cast, this just becomes a memcpy.
   if (dstType->getUnqualified() == srcType->getUnqualified())
   {
      make_objects_memcpy_post(objects, dst, src, srcType, position);

      return;
   }

   VariableType::BasicType dstBT = dstType->getBasicType();
   VariableType::BasicType srcBT = srcType->getBasicType();

   // If casting to void, just memcpy to void.
   if (dstBT == VariableType::BT_VOID)
   {
      make_objects_memcpy_void(objects, src, position);

      // If there's an offset temp, it needs to be dealt with.
      if (dst->offsetTemp)
         make_objects_memcpy_void(objects, dst->offsetTemp, position);

      return;
   }

   // All casting is stack-ops.
   VariableData::Pointer tmp = VariableData::create_stack(src->size);

   make_objects_memcpy_prep(objects, tmp, src, position);
   make_objects_memcpy_post(objects, tmp, src, srcType, position);

   switch (srcBT)
   {
   case VariableType::BT_ARRAY:
   case VariableType::BT_ASMFUNC:
   case VariableType::BT_BLOCK:
   case VariableType::BT_FLOAT:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_VOID:
      throw SourceException(TYPES_STRING, position, __func__);

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_SHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      switch (dstBT)
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
         objects->addToken(OCODE_LOGICAL_NOT32I);
         objects->addToken(OCODE_LOGICAL_NOT32I);
         break;

      case VariableType::BT_BOOLSOFT:
      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_REAL:
         objects->addToken(OCODE_CONVERT_32I_32F);
         break;

      case VariableType::BT_LLONG:
         if (VariableType::is_bt_unsigned(srcBT))
         {
            objects->addTokenPushZero();
            break;
         }

      {
         // Must sign-extend. Which means... BRANCHING! AAAAAAAA!
         std::string labelPos = context->makeLabel();
         std::string labelEnd = context->makeLabel();
         objects->addToken(OCODE_STACK_DUP32);
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
         objects->addToken(OCODE_BITWISE_AND32);
         objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelPos));
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0xFFFFFFFF));
         objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));
         objects->addLabel(labelPos);
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x00000000));
         objects->addLabel(labelEnd);
      }
         break;

      case VariableType::BT_ULLONG:
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_BOOLSOFT:
      switch (dstBT)
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         objects->addToken(OCODE_LOGICAL_NOT32I);
         objects->addToken(OCODE_LOGICAL_NOT32I);
         break;

      case VariableType::BT_BOOLSOFT:
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_REAL:
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_CONVERT_32I_32F);
         break;

      case VariableType::BT_LLONG:
      case VariableType::BT_ULLONG:
         objects->addToken(OCODE_LOGICAL_NOT32I);
         objects->addToken(OCODE_LOGICAL_NOT32I);
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_FIXED:
   case VariableType::BT_REAL:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_LOGICAL_NOT32F);
         break;

      case VariableType::BT_BOOLSOFT:
      case VariableType::BT_FIXED:
      case VariableType::BT_REAL:
         break;

      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         objects->addToken(OCODE_CONVERT_32F_32I);
         break;
      case VariableType::BT_LLONG:
      {
         // Must sign-extend. Which means... BRANCHING! AAAAAAAA!
         std::string labelPos = context->makeLabel();
         std::string labelEnd = context->makeLabel();
         objects->addToken(OCODE_STACK_DUP32);
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
         objects->addToken(OCODE_BITWISE_AND32);
         objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelPos));
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0xFFFFFFFF));
         objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));
         objects->addLabel(labelPos);
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x00000000));
         objects->addLabel(labelEnd);
      }
         break;

      case VariableType::BT_ULLONG:
         objects->addTokenPushZero();
         break;
      }
      break;

   case VariableType::BT_LLONG:
   case VariableType::BT_ULLONG:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_FLOAT:
      case VariableType::BT_LFLOAT:
      case VariableType::BT_LLFLOAT:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
         objects->addToken(OCODE_LOGICAL_IOR32I);
         break;

      case VariableType::BT_BOOLSOFT:
         objects->addToken(OCODE_BITWISE_IOR32);
         break;

      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_LONG:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCHAR:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_SHORT:
      case VariableType::BT_STRING:
      case VariableType::BT_UCHAR:
      case VariableType::BT_UINT:
      case VariableType::BT_ULONG:
      case VariableType::BT_USHORT:
         objects->addToken(OCODE_STACK_DROP32);
         break;

      case VariableType::BT_FIXED:
      case VariableType::BT_REAL:
         // What about the high byte, you ask? If the high byte is meaningful,
         // we've overflowed the fixed anyway. Therefore: Undefined behavior.
         objects->addToken(OCODE_STACK_DROP32);
         objects->addToken(OCODE_CONVERT_32I_32F);
         break;

      case VariableType::BT_LLONG:
      case VariableType::BT_ULLONG:
         break;
      }
      break;
   }

   make_objects_memcpy_post(objects, dst, tmp, dstType, position);

   #undef TYPES_STRING
}

// EOF

