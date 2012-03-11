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

   //
   // ::canGetData
   //
   virtual bool canGetData() const
   {
      return exprL->getType()->getBasicType() == VariableType::BT_ARRAY;
   }

   //
   // ::getData
   //
   virtual VariableData::Pointer getData() const
   {
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(position);

      ObjectExpression::Pointer obj = exprL->makeObject();
      obj = obj->resolveElement(exprR->makeObject()->resolveInt());

      return VariableData::create_literal(size, obj);
   }

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
      (btL == VariableType::BT_ARRAY && !exprL->canMakeObject()) ||
      (btR == VariableType::BT_ARRAY && !exprR->canMakeObject()))
   {
      return create_unary_dereference
      (create_binary_add(exprL, exprR, context, position), context, position);
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
      throw SourceException
      ("expected BT_ARRAY or BT_STRING for exprL got " + make_string(btL),
       position, getClassName());
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
      VariableType::Reference type = getType();
      bigsint typeSize = type->getSize(position);
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

      make_objects_memcpy_post(objects, dst, src, type, context, position);
   }
   else
   {
      throw SourceException("invalid BT", position, getClassName());
   }
}

// EOF

