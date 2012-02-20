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
SourceExpression::
SourceExpression(SourceContext *_context, SourcePosition const &_position)
                 : position(_position), label(_context->makeLabel()),
                   context(_context), evaluated(false)
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
   return canGetData() && getData()->type == VariableData::MT_LITERAL;
}

//
// SourceExpression::get_promoted_type
//
VariableType::Reference SourceExpression::
get_promoted_type(VariableType *type1, VariableType *type2,
                  SourcePosition const &)
{
   if (type1 == type2) return static_cast<VariableType::Reference>(type1);

   VariableType::BasicType bt1 = type1->getBasicType();
   VariableType::BasicType bt2 = type2->getBasicType();

   if (bt1 == VariableType::BT_VOID) return type1->getUnqualified();
   if (bt2 == VariableType::BT_VOID) return type2->getUnqualified();

   if (bt1 == VariableType::BT_POINTER) return type1->getUnqualified();
   if (bt2 == VariableType::BT_POINTER) return type2->getUnqualified();

   if (bt1 == VariableType::BT_ARRAY) return type1->getReturn()->getPointer();
   if (bt2 == VariableType::BT_ARRAY) return type2->getReturn()->getPointer();

   if (bt1 == VariableType::BT_REAL) return type1->getUnqualified();
   if (bt2 == VariableType::BT_REAL) return type2->getUnqualified();

   if (bt1 == VariableType::BT_INT) return type1->getUnqualified();
   if (bt2 == VariableType::BT_INT) return type2->getUnqualified();

   if (bt1 == VariableType::BT_UINT) return type1->getUnqualified();
   if (bt2 == VariableType::BT_UINT) return type2->getUnqualified();

   if (bt1 == VariableType::BT_CHAR) return type1->getUnqualified();
   if (bt2 == VariableType::BT_CHAR) return type2->getUnqualified();

   if (bt1 == VariableType::BT_STRING) return type1->getUnqualified();
   if (bt2 == VariableType::BT_STRING) return type2->getUnqualified();

   if (bt1 == VariableType::BT_BOOLHARD) return type1->getUnqualified();
   if (bt2 == VariableType::BT_BOOLHARD) return type2->getUnqualified();

   if (bt1 == VariableType::BT_BOOLSOFT) return type1->getUnqualified();
   if (bt2 == VariableType::BT_BOOLSOFT) return type2->getUnqualified();

   return VariableType::get_bt_void();
}

//
// SourceExpression::getData
//
VariableData::Pointer SourceExpression::getData() const
{
   throw SourceException("getData on invalid expression", position, getName());
}

//
// SourceExpression::getType
//
VariableType::Reference SourceExpression::getType() const
{
   return VariableType::get_bt_void();
}

//
// SourceExpression::makeObject
//
CounterPointer<ObjectExpression> SourceExpression::makeObject() const
{
   VariableData::Pointer src = getData();

   if (src->type != VariableData::MT_LITERAL)
      throw SourceException("makeObject on invalid expression", position,
                            getName());

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

      VariableData::SectionL section;

      VariableType::Reference type = getType();

      if (type->getBasicType() == VariableType::BT_STRING)
         section = VariableData::SL_STRING;
      else
         section = VariableData::SL_INT;

      VariableData::Pointer src =
         VariableData::create_literal(type->getSize(position), section,
                                      makeObject());

      make_objects_memcpy_prep(objects, dst, src, position);
      make_objects_memcpy_post(objects, dst, src, position);
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
   //if (evaluated)
   //   throw SourceException("multiple-evaluation", position, getName());

   evaluated = true;

   objects->addLabel(labels);
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
         make_objects_memcpy_void(objects, dst->offsetTemp, position);
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
   objects->setPosition(position);
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

   VariableData::Pointer src = getData();

   make_objects_memcpy_prep(objects, dst, src, position);
   make_objects_memcpy_post(objects, dst, src, position);
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

      make_objects_memcpy_prep(objects, dst, src, position);
      make_objects_memcpy_cast(objects, dst, src, dstType, srcType, position);
   }
   else
   {
      VariableData::Pointer src =
         VariableData::create_stack(srcType->getSize(position));

      make_objects_memcpy_prep(objects, dst, src, position);
      virtual_makeObjects(objects, src);
      make_objects_memcpy_cast(objects, dst, src, dstType, srcType, position);
   }
}

// EOF

