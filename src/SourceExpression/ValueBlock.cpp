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

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



//
// SourceExpression_ValueBlock
//
class SourceExpression_ValueBlock : public SourceExpression
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueBlock, SourceExpression);

public:
   SourceExpression_ValueBlock(
      std::vector<SourceExpression::Pointer> const &expressions,
      SourcePosition const &position);

   virtual VariableType const * getType() const;

private:
   void makeVoid(ObjectVector *objects) const;

   virtual void
   virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   virtual void
   virtual_makeObjectsCast(ObjectVector *objects, VariableData *dst,
                           VariableType const *dstType);

   std::vector<SourceExpression::Pointer> expressions;
   VariableType const *type;
};



//
// SourceExpression::create_value_block
//
SourceExpression::Pointer SourceExpression::
create_value_block(std::vector<SourceExpression::Pointer> const &expressions,
                   SourcePosition const & position)
{
   return new SourceExpression_ValueBlock(expressions, position);
}

//
// SourceExpression_ValueBlock::SourceExpression_ValueBlock
//
SourceExpression_ValueBlock::
SourceExpression_ValueBlock(
   std::vector<SourceExpression::Pointer> const &_expressions,
   SourcePosition const &_position)
   : Super(_position), expressions(_expressions)
{
   std::vector<VariableType const *> types(expressions.size());

   for (size_t i(0); i < expressions.size(); ++i)
      types[i] = expressions[i]->getType();

   type = VariableType::get_block(types);
}

//
// SourceExpression_ValueBlock::getType
//
VariableType const * SourceExpression_ValueBlock::getType() const
{
   return type;
}

//
// SourceExpression_ValueBlock::makeVoid
//
void SourceExpression_ValueBlock::makeVoid(ObjectVector *objects) const
{
   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType const   *dstPartType;

   for (size_t i(0); i < expressions.size(); ++i)
   {
      dstPartType = expressions[i]->getType();
      dstPartSize = dstPartType->size(position);
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

   bigsint               tmpSize = type->size(position);
   VariableData::Pointer tmp     = VariableData::create_stack(tmpSize);

   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType const   *dstPartType;

   make_objects_memcpy_prep(objects, dst, tmp, position);

   for (size_t i(0); i < expressions.size(); ++i)
   {
      dstPartType = expressions[i]->getType();
      dstPartSize = dstPartType->size(position);
      dstPart = VariableData::create_stack(dstPartSize);
      expressions[i]->makeObjects(objects, dstPart);
   }

   make_objects_memcpy_post(objects, dst, tmp, position);
}

//
// SourceExpression_ValueBlock::virtual_makeObjectsCast
//
void SourceExpression_ValueBlock::
virtual_makeObjectsCast(ObjectVector *objects, VariableData *dst,
                        VariableType const *dstType)
{
   Super::recurse_makeObjectsCast(objects, dst, dstType);

   if (dst->type == VariableData::MT_VOID
    || dstType->vt == VariableType::VT_VOID)
   {
      makeVoid(objects);
      return;
   }

   if (expressions.size() != dstType->types.size())
      throw SourceException("incorrect number of expressions to cast",
                            position, getName());

   bigsint               tmpSize = type->size(position);
   VariableData::Pointer tmp     = VariableData::create_stack(tmpSize);

   VariableData::Pointer dstPart;
   bigsint               dstPartSize;
   VariableType const   *dstPartType;

   make_objects_memcpy_prep(objects, dst, tmp, position);

   for (size_t i(0); i < expressions.size(); ++i)
   {
      dstPartType = expressions[i]->getType();
      dstPartSize = dstPartType->size(position);
      dstPart = VariableData::create_stack(dstPartSize);
      expressions[i]->makeObjectsCast(objects, dstPart, dstType->types[i]);
   }

   make_objects_memcpy_post(objects, dst, tmp, position);
}

// EOF

