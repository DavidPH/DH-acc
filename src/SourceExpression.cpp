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
// Base class for source-level expression handling.
//
//-----------------------------------------------------------------------------

#include "SourceExpression.hpp"

#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::SourceExpression
//
SourceExpression::SourceExpression(SRCEXP_EXPR_PARM)
 : pos(_pos), context(_context), evaluated(false)
{
}

//
// SourceExpression::~SourceExpression
//
SourceExpression::~SourceExpression()
{
}

//
// SourceExpression::addLabel
//
void SourceExpression::addLabel(std::string const &_label)
{
   labels.push_back(_label);
}

//
// SourceExpression::canGetData
//
bool SourceExpression::canGetData() const
{
   return false;
}

//
// SourceExpression::canMakeObject
//
bool SourceExpression::canMakeObject() const
{
   if (!canGetData()) return false;

   VariableData::Pointer data = getData();

   if (data->type != VariableData::MT_LITERAL)
      return false;

   return true;
}

//
// SourceExpression::get_promoted_type
//
VariableType::Reference SourceExpression::
get_promoted_type(VariableType *_type1, VariableType *_type2,
                  SourcePosition const &)
{
   VariableType::Reference type1(_type1);
   VariableType::Reference type2(_type2);

   VariableType::BasicType bt1 = type1->getBasicType();
   VariableType::BasicType bt2 = type2->getBasicType();

   // Arrays have special promotion.
   if (bt1 == VariableType::BT_ARR && bt2 == VariableType::BT_ARR)
   {
      if (type1 == type2) return type1;

      return VariableType::get_bt_void();
   }

   // Decay arrays to pointers and de-qualify everything else.
   if (bt1 == VariableType::BT_ARR)
   {
      type1 = type1->getReturn()->getPointer();
      bt1 = VariableType::BT_PTR;
   }
   else
      type1 = type1->getUnqualified();

   if (bt2 == VariableType::BT_ARR)
   {
      type2 = type2->getReturn()->getPointer();
      bt2 = VariableType::BT_PTR;
   }
   else
      type2 = type2->getUnqualified();

   // nullptr should always try to promote to the other type.
   if (bt1 == VariableType::BT_PTR_NUL) return type2;
   if (bt2 == VariableType::BT_PTR_NUL) return type1;

   // Pointers have special promotion.
   if (bt1 == VariableType::BT_PTR && bt2 == VariableType::BT_PTR)
   {
      if (type1 == type2) return type1;

      VariableType::Reference retn1 = type1->getReturn();
      VariableType::Reference retn2 = type2->getReturn();

      // If one is more qualified than the other, use it.
      if (retn1->getQualifiers())
      {
         if (!retn2->getQualifiers())
            return type1->getPointer();

         if (!retn2->getQualifier(retn1->getQualifiers()))
            return type1;

         return type2;
      }
      else if (retn2->getQualifiers())
         return type2;

      // Neither has qualifiers, so go by the basic types.
      bt1 = retn1->getBasicType();
      bt2 = retn2->getBasicType();

      // Promote to void.
      if (bt1 == VariableType::BT_VOID) return type1;
      if (bt2 == VariableType::BT_VOID) return type2;

      // Nothing seems better, so just return the left-hand type.
      return type1;
   }

   if (bt1 == VariableType::BT_VOID) return type1;
   if (bt2 == VariableType::BT_VOID) return type2;

   if (bt1 == VariableType::BT_PTR) return type1;
   if (bt2 == VariableType::BT_PTR) return type2;

   if (bt1 == VariableType::BT_FLT_LL) return type1;
   if (bt2 == VariableType::BT_FLT_LL) return type2;

   if (bt1 == VariableType::BT_FLT_L) return type1;
   if (bt2 == VariableType::BT_FLT_L) return type2;

   if (bt1 == VariableType::BT_FLT) return type1;
   if (bt2 == VariableType::BT_FLT) return type2;

   if (bt1 == VariableType::BT_FLT_H) return type1;
   if (bt2 == VariableType::BT_FLT_H) return type2;

   if (bt1 == VariableType::BT_FLT_HH) return type1;
   if (bt2 == VariableType::BT_FLT_HH) return type2;

   if (bt1 == VariableType::BT_FIX_LL) return type1;
   if (bt2 == VariableType::BT_FIX_LL) return type2;

   if (bt1 == VariableType::BT_FIX_L) return type1;
   if (bt2 == VariableType::BT_FIX_L) return type2;

   if (bt1 == VariableType::BT_FIX) return type1;
   if (bt2 == VariableType::BT_FIX) return type2;

   if (bt1 == VariableType::BT_FIX_H) return type1;
   if (bt2 == VariableType::BT_FIX_H) return type2;

   if (bt1 == VariableType::BT_FIX_HH) return type1;
   if (bt2 == VariableType::BT_FIX_HH) return type2;

   if (bt1 == VariableType::BT_INT_LL) return type1;
   if (bt2 == VariableType::BT_INT_LL) return type2;

   if (bt1 == VariableType::BT_UNS_LL) return type1;
   if (bt2 == VariableType::BT_UNS_LL) return type2;

   if (bt1 == VariableType::BT_INT_L) return type1;
   if (bt2 == VariableType::BT_INT_L) return type2;

   if (bt1 == VariableType::BT_UNS_L) return type1;
   if (bt2 == VariableType::BT_UNS_L) return type2;

   if (bt1 == VariableType::BT_INT) return type1;
   if (bt2 == VariableType::BT_INT) return type2;

   if (bt1 == VariableType::BT_UNS) return type1;
   if (bt2 == VariableType::BT_UNS) return type2;

   if (bt1 == VariableType::BT_INT_H) return type1;
   if (bt2 == VariableType::BT_INT_H) return type2;

   if (bt1 == VariableType::BT_UNS_H) return type1;
   if (bt2 == VariableType::BT_UNS_H) return type2;

   if (bt1 == VariableType::BT_INT_HH) return type1;
   if (bt2 == VariableType::BT_INT_HH) return type2;

   if (bt1 == VariableType::BT_UNS_HH) return type1;
   if (bt2 == VariableType::BT_UNS_HH) return type2;

   if (bt1 == VariableType::BT_CHR) return type1;
   if (bt2 == VariableType::BT_CHR) return type2;

   if (bt1 == VariableType::BT_ENUM) return VariableType::get_bt_int();
   if (bt2 == VariableType::BT_ENUM) return VariableType::get_bt_int();

   if (bt1 == VariableType::BT_STR) return type1;
   if (bt2 == VariableType::BT_STR) return type2;

   if (bt1 == VariableType::BT_BIT_HRD) return type1;
   if (bt2 == VariableType::BT_BIT_HRD) return type2;

   if (bt1 == VariableType::BT_BIT_SFT) return type1;
   if (bt2 == VariableType::BT_BIT_SFT) return type2;

   return VariableType::get_bt_void();
}

//
// SourceExpression::getData
//
VariableData::Pointer SourceExpression::getData() const
{
   ERROR_NP("getData on invalid expression");
}

//
// SourceExpression::getType
//
VariableType::Reference SourceExpression::getType() const
{
   return VariableType::get_bt_void();
}

//
// SourceExpression::makeExpressionFunction
//
SourceExpression::Pointer SourceExpression::makeExpressionFunction
(VariableType::Vector const &)
{
   VariableType::Reference type = getType();

   if (VariableType::is_bt_function(type->getBasicType()))
      return this;

   ERROR_NP("makeExpressionFunction on invalid expression");
}

//
// SourceExpression::makeObject
//
CounterPointer<ObjectExpression> SourceExpression::makeObject() const
{
   VariableData::Pointer src = getData();

   if (src->type != VariableData::MT_LITERAL)
      ERROR_NP("makeObject on invalid expression");

   return src->address;
}

//
// SourceExpression::makeObjects
//
void SourceExpression::
makeObjects(ObjectVector *objects, VariableData *dst)
{
   makeObjectsBase(objects, dst);

   if (canMakeObject())
   {
      recurse_makeObjects(objects, dst);

      VariableType::Reference type = getType();

      // Don't try to makeObject on a void expression.
      if (type->getBasicType() == VariableType::BT_VOID) return;

      VariableData::Pointer src =
         VariableData::create_literal(type->getSize(pos), makeObject());

      make_objects_memcpy_prep(objects, dst, src, pos);
      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }
   else
      virtual_makeObjects(objects, dst);
}

//
// SourceExpression::makeObjectsBase
//
void SourceExpression::
makeObjectsBase(ObjectVector *objects, VariableData *)
{
   if (!labels.empty())
   {
      if (evaluated)
         ERROR_NP("multiple-evaluation of labelled expression");

      objects->addLabel(labels);
   }

   evaluated = true;
}

//
// SourceExpression::makeObjectsCast
//
void SourceExpression::makeObjectsCast
(ObjectVector *objects, VariableData *dst, VariableType *type)
{
   // If casting to void, just use a void destination for better codegen.
   if (type->getBasicType() == VariableType::BT_VOID && !dst->offsetTemp)
   {
      makeObjects(objects, VariableData::create_void(dst->size));

      // If there's an offset temp, it needs to be dealt with. (Well, only if
      // it's on the stack, but this ensures anything is handled.)
      if (dst->offsetTemp)
         make_objects_memcpy_void(objects, dst->offsetTemp, pos);
   }
   else
   {
      makeObjectsBase(objects, dst);

      virtual_makeObjectsCast(objects, dst, type);
   }
}

//
// SourceExpression::recurse_makeObjects
//
void SourceExpression::
recurse_makeObjects(ObjectVector *objects, VariableData *dst)
{
   recurse_makeObjectsBase(objects, dst);
}

//
// SourceExpression::recurse_makeObjectsBase
//
void SourceExpression::
recurse_makeObjectsBase(ObjectVector *objects, VariableData *)
{
   objects->setPosition(pos);
}

//
// SourceExpression::recurse_makeObjectsCast
//
void SourceExpression::recurse_makeObjectsCast
(ObjectVector *objects, VariableData *dst, VariableType *)
{
   recurse_makeObjectsBase(objects, dst);
}

//
// SourceExpression::virtual_makeObjects
//
void SourceExpression::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   recurse_makeObjects(objects, dst);

   VariableData::Pointer   src  = getData();
   VariableType::Reference type = getType();

   make_objects_memcpy_prep(objects, dst, src, pos);
   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

//
// SourceExpression::virtual_makeObjectsCast
//
void SourceExpression::virtual_makeObjectsCast
(ObjectVector * objects, VariableData *dst, VariableType *dstType)
{
   VariableType::Reference srcType = getType();

   if (canGetData())
   {
      VariableData::Pointer src = getData();

      make_objects_memcpy_prep(objects, dst, src, pos);
      make_objects_memcpy_cast
         (objects, dst, src, dstType, srcType, context, pos);
   }
   else
   {
      VariableData::Pointer src =
         VariableData::create_stack(srcType->getSize(pos));

      make_objects_memcpy_prep(objects, dst, src, pos);
      virtual_makeObjects(objects, src);
      make_objects_memcpy_cast
         (objects, dst, src, dstType, srcType, context, pos);
   }
}

// EOF

