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
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


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
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_STRING:
   case VariableType::BT_UINT:
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
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
         break;

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
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         break;

      case VariableType::BT_BOOLSOFT:
         break;

      case VariableType::BT_REAL:
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_branch_not(obj, position);
         obj = ObjectExpression::create_cast_int_to_float(obj, position);
         break;
      }
      break;

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

      case VariableType::BT_REAL:
         break;

      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
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
 VariableType *dstType, VariableType *srcType, SourcePosition const &position)
{
   // Don't want to slog through all that if not throwing, but also want to
   // avoid code duplication.
   #define TYPES_STRING ("bad BT " + make_string(srcType->getBasicType()) + \
                         "->"      + make_string(dstType->getBasicType()))

   // If no cast, this just becomes a memcpy.
   if (dstType->getUnqualified() == srcType->getUnqualified())
   {
      make_objects_memcpy_post(objects, dst, src, position);

      return;
   }

   // If casting to void, just memcpy to void.
   if (dstType->getBasicType() == VariableType::BT_VOID)
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
   make_objects_memcpy_post(objects, tmp, src, position);

   switch (srcType->getBasicType())
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
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_STRING:
   case VariableType::BT_UINT:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
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
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
         break;

      case VariableType::BT_REAL:
         objects->addToken(OCODE_CONVERT_32I_32F);
         break;
      }
      break;

   case VariableType::BT_BOOLSOFT:
      switch (dstType->getBasicType())
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
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
         objects->addToken(OCODE_LOGICAL_NOT32I);
         objects->addToken(OCODE_LOGICAL_NOT32I);
         break;

      case VariableType::BT_BOOLSOFT:
         break;

      case VariableType::BT_REAL:
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_CONVERT_32I_32F);
         break;
      }
      break;

   case VariableType::BT_REAL:
      switch (dstType->getBasicType())
      {
      case VariableType::BT_ARRAY:
      case VariableType::BT_ASMFUNC:
      case VariableType::BT_BLOCK:
      case VariableType::BT_STRUCT:
      case VariableType::BT_UNION:
      case VariableType::BT_VOID:
         throw SourceException(TYPES_STRING, position, __func__);

      case VariableType::BT_BOOLHARD:
         objects->addToken(OCODE_LOGICAL_NOT32F);
         objects->addToken(OCODE_LOGICAL_NOT32F);
         break;

      case VariableType::BT_BOOLSOFT:
      case VariableType::BT_REAL:
         break;

      case VariableType::BT_CHAR:
      case VariableType::BT_ENUM:
      case VariableType::BT_FUNCTION:
      case VariableType::BT_INT:
      case VariableType::BT_LABEL:
      case VariableType::BT_LINESPEC:
      case VariableType::BT_NATIVE:
      case VariableType::BT_POINTER:
      case VariableType::BT_SCRIPT:
      case VariableType::BT_STRING:
      case VariableType::BT_UINT:
         objects->addToken(OCODE_CONVERT_32F_32I);
         break;
      }
      break;
   }

   make_objects_memcpy_post(objects, dst, tmp, position);

   #undef TYPES_STRING
}

// EOF

