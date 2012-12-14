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
// SourceExpression handling of blocks of expressions.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueBlock
//
class SourceExpression_ValueBlock : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueBlock,
                                   SourceExpression);

public:
   SourceExpression_ValueBlock(Vector const &args, SRCEXP_EXPR_ARGS);

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      for (Vector::const_iterator iter = expressions.begin();
           iter != expressions.end(); ++iter)
         if (!(*iter)->canMakeObject()) return false;

      return true;
   }

   virtual VariableType::Reference getType() const;

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      for(Vector::const_iterator itr = expressions.begin(),
          end = expressions.end(); itr != end; ++itr)
      {
         if((*itr)->isSideEffect())
            return true;
      }

      return false;
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      ObjectExpression::Vector elems;

      for (Vector::const_iterator iter = expressions.begin();
           iter != expressions.end(); ++iter)
         elems.push_back((*iter)->makeObject());

      return ObjectExpression::create_value_arr(elems, pos);
   }

   //
   // makeObjectPartial
   //
   virtual ObjectExpression::Pointer makeObjectPartial() const
   {
      ObjectExpression::Vector elems;

      for(Vector::const_iterator itr = expressions.begin(),
          end = expressions.end(); itr != end; ++itr)
      {
         elems.push_back((*itr)->makeObjectPartial());
      }

      return ObjectExpression::create_value_arr(elems, pos);
   }

private:
   void makeVoid(ObjectVector *objects) const;

   //
   // ::makeZero
   //
   void makeZero(ObjectVector *objects, VariableType *dstPartType) const
   {
      for (bigsint i = dstPartType->getSize(pos); i--;)
         objects->addTokenPushZero();
   }

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   virtual void virtual_makeObjectsCast
   (ObjectVector *objects, VariableData *dst, VariableType *dstType);

   Vector expressions;
   VariableType::Pointer type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_block
//
SRCEXP_EXPRVAL_DEFN(a, block)
{
   return new SourceExpression_ValueBlock(args, context, pos);
}

//
// SourceExpression_ValueBlock::SourceExpression_ValueBlock
//
SourceExpression_ValueBlock::SourceExpression_ValueBlock
(Vector const &args, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS), expressions(args)
{
   VariableType::Vector types(expressions.size());

   for (size_t i = 0; i < expressions.size(); ++i)
      types[i] = expressions[i]->getType();

   type = VariableType::get_bt_block(types);
}

//
// SourceExpression_ValueBlock::getType
//
VariableType::Reference SourceExpression_ValueBlock::getType() const
{
   return static_cast<VariableType::Reference>(type);
}

//
// SourceExpression_ValueBlock::makeVoid
//
void SourceExpression_ValueBlock::makeVoid(ObjectVector *objects) const
{
   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType::Pointer dstPartType;

   for (size_t i = 0; i < expressions.size(); ++i)
   {
      dstPartType = expressions[i]->getType();
      dstPartSize = dstPartType->getSize(pos);
      dstPart = VariableData::create_void(dstPartSize);
      expressions[i]->makeObjects(objects, dstPart);
   }
}

//
// SourceExpression_ValueBlock::virtual_makeObjects
//
void SourceExpression_ValueBlock::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (dst->type == VariableData::MT_VOID)
   {
      makeVoid(objects);
      return;
   }

   bigsint               tmpSize = type->getSize(pos);
   VariableData::Pointer tmp     = VariableData::create_stack(tmpSize);

   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType::Pointer dstPartType;

   make_objects_memcpy_prep(objects, dst, tmp, pos);

   for (size_t i = 0; i < expressions.size(); ++i)
   {
      dstPartType = expressions[i]->getType();
      dstPartSize = dstPartType->getSize(pos);
      dstPart = VariableData::create_stack(dstPartSize);
      expressions[i]->makeObjects(objects, dstPart);
   }

   make_objects_memcpy_post(objects, dst, tmp, type, context, pos);
}

//
// SourceExpression_ValueBlock::virtual_makeObjectsCast
//
void SourceExpression_ValueBlock::virtual_makeObjectsCast
(ObjectVector *objects, VariableData *dst, VariableType *dstType)
{
   Super::recurse_makeObjectsCast(objects, dst, dstType);

   if (dst->type == VariableData::MT_VOID
    || dstType->getBasicType() == VariableType::BT_VOID)
   {
      makeVoid(objects);
      return;
   }

   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType::Pointer dstPartType;

   bigsint               tmpSize = type->getSize(pos);
   VariableData::Pointer tmp     = VariableData::create_stack(tmpSize);

   if (dstType->getBasicType() == VariableType::BT_ARR)
   {
      make_objects_memcpy_prep(objects, dst, tmp, pos);

      bigsint dstTypes = dstType->getWidth();

      dstPartType = dstType->getReturn();
      dstPartSize = dstPartType->getSize(pos);
      dstPart = VariableData::create_stack(dstPartSize);

      for (bigsint i = 0; i < dstTypes; ++i)
      {
         if (static_cast<size_t>(i) < expressions.size())
            expressions[i]->makeObjectsCast(objects, dstPart, dstPartType);
         else
            makeZero(objects, dstPartType);
      }

      make_objects_memcpy_post(objects, dst, tmp, dstType, context, pos);
   }
   else if (dstType->getBasicType() == VariableType::BT_STRUCT)
   {
      make_objects_memcpy_prep(objects, dst, tmp, pos);

      VariableType::Vector const &dstTypes = dstType->getTypes();

      for (size_t i = 0; i < dstTypes.size(); ++i)
      {
         dstPartType = dstTypes[i];
         dstPartSize = dstPartType->getSize(pos);
         dstPart = VariableData::create_stack(dstPartSize);

         if (i < expressions.size())
            expressions[i]->makeObjectsCast(objects, dstPart, dstPartType);
         else
            makeZero(objects, dstPartType);
      }

      make_objects_memcpy_post(objects, dst, tmp, dstType, context, pos);
   }
   else if (!expressions.empty())
   {
      Vector::iterator itr, end = expressions.end()-1;

      for (itr = expressions.begin(); itr != end; ++itr)
      {
         dstPartType = (*itr)->getType();
         dstPartSize = dstPartType->getSize(pos);
         dstPart = VariableData::create_void(dstPartSize);
         (*itr)->makeObjects(objects, dstPart);
      }

      (*itr)->makeObjectsCast(objects, dst, dstType);
   }
   else
   {
      make_objects_memcpy_prep(objects, dst, tmp, pos);

      makeZero(objects, dstType);

      make_objects_memcpy_post(objects, dst, tmp, dstType, context, pos);
   }
}

// EOF

