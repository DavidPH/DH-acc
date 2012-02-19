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
// SourceExpression handling of "operator []".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryArray
//
class SourceExpression_BinaryArray : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryArray(SRCEXP_EXPRBIN_ARGS);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType::Reference getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_array
//
SRCEXP_EXPRBIN_DEFN(array)
{
   VariableType::Reference typeL = exprL->getType();
   VariableType::Reference typeR = exprR->getType();

   VariableType::BasicType btL = typeL->getBasicType();
   VariableType::BasicType btR = typeR->getBasicType();

   // This allows C semantics for array access. Specifically that x[y] be the
   // same as *(x+y).
   if (btL == VariableType::BT_POINTER || btR == VariableType::BT_POINTER ||
      (btL == VariableType::BT_ARRAY && exprL->canMakeObjectsAddress()) ||
      (btR == VariableType::BT_ARRAY && exprR->canMakeObjectsAddress())
   )
   {
      return create_unary_dereference
             (create_binary_add(exprL, exprR, context, position), context,
              position);
   }

   return new SourceExpression_BinaryArray(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryArray::SourceExpression_BinaryArray
//
SourceExpression_BinaryArray::SourceExpression_BinaryArray
(SRCEXP_EXPRBIN_PARM)
 : Super(NULL, VariableType::get_bt_uint(), SRCEXP_EXPRBIN_PASS)
{
   VariableType::BasicType btL = exprL->getType()->getBasicType();

   // Can only be done for BT_ARRAY or BT_STRING.
   if (btL != VariableType::BT_ARRAY && btL != VariableType::BT_STRING)
      throw SourceException("expected BT_ARRAY or BT_STRING for exprL got "
                            + make_string(btL), position, getName());
}

//
// SourceExpression_BinaryArray::canGetData
//
bool SourceExpression_BinaryArray::canGetData() const
{
   return true;
}

//
// SourceExpression_BinaryArray::getData
//
VariableData::Pointer SourceExpression_BinaryArray::getData() const
{
   #define PARM context, position

   VariableData::Pointer src = exprL->getData();
   VariableType::Reference type = getType();
   bigsint typeSize = type->getSize(position);

   if (src->type != VariableData::MT_REGISTERARRAY)
      throw SourceException("cannot getData", position, getName());

   SourceExpression::Pointer offset = exprR;

   // If the type's size isn't 1, need to multiply the offset.
   if (typeSize != 1)
   {
      offset = create_binary_mul
               (offset, create_value_uint(typeSize, PARM), PARM);
   }

   // If there is already an offset, add it.
   if (src->offsetExpr)
      offset = create_binary_add(src->offsetExpr, offset, PARM);

   return VariableData::create_registerarray
          (typeSize, src->sectionRA, src->address, offset);

   #undef PARM
}

//
// SourceExpression_BinaryArray::getType
//
VariableType::Reference SourceExpression_BinaryArray::getType() const
{
   return exprL->getType()->getReturn();
}

//
// SourceExpression_BinaryArray::virtual_makeObjects
//
void SourceExpression_BinaryArray::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (exprL->getType()->getBasicType() == VariableType::BT_STRING)
   {
      bigsint typeSize = getType()->getSize(position);
      bigsint sizeL = exprL->getType()->getSize(position);
      bigsint sizeR = exprR->getType()->getSize(position);

      VariableData::Pointer src = VariableData::create_stack(typeSize);

      make_objects_memcpy_prep(objects, dst, src, position);

      exprL->makeObjects(objects, VariableData::create_stack(sizeL));
      exprR->makeObjects(objects, VariableData::create_stack(sizeR));

      objects->setPosition(position);

      // 2 = arg count, 15 = native get_char
      objects->addToken(OCODE_MISC_NATIVE, objects->getValue(2),
                        objects->getValue(15));

      make_objects_memcpy_post(objects, dst, src, position);
   }
   else
   {
      VariableData::Pointer src = getData();

      make_objects_memcpy_prep(objects, dst, src, position);
      make_objects_memcpy_post(objects, dst, src, position);
   }
}

// EOF

