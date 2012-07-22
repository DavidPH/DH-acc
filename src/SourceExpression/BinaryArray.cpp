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
      return exprL->getType()->getBasicType() == VariableType::BT_ARR;
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      if (exprL->getType()->getBasicType() == VariableType::BT_STR)
      {
         return exprL->canMakeObject() && exprR->canMakeObject() &&
                exprR->makeObject()->canResolve();
      }
      else
         return false;
   }

   //
   // ::getData
   //
   virtual VariableData::Pointer getData() const
   {
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(pos);

      ObjectExpression::Pointer obj = exprL->makeObject();
      obj = obj->resolveARR(exprR->makeObject()->resolveUNS());

      return VariableData::create_literal(size, obj);
   }

   virtual VariableType::Reference getType() const;

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      if (exprL->getType()->getBasicType() == VariableType::BT_STR)
      {
         std::string string = exprL->makeObject()->resolveString();
         biguint     index  = exprR->makeObject()->resolveUNS();

         if(index >= string.size())
            return ObjectExpression::create_value_int(0, pos);

         return ObjectExpression::create_value_int(string[index], pos);
      }
      else
         ERROR_NP("invalid BT");
   }

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
   if (btL == VariableType::BT_PTR || btR == VariableType::BT_PTR ||
      (btL == VariableType::BT_ARR && !exprL->canMakeObject()) ||
      (btR == VariableType::BT_ARR && !exprR->canMakeObject()))
   {
      return create_unary_dereference
      (create_binary_add(exprL, exprR, context, pos), context, pos);
   }

   return new SourceExpression_BinaryArray(exprL, exprR, context, pos);
}

//
// SourceExpression_BinaryArray::SourceExpression_BinaryArray
//
SourceExpression_BinaryArray::SourceExpression_BinaryArray(SRCEXP_EXPRBIN_PARM)
 : Super(SRCEXP_EXPRBIN_PASS, NULL, VariableType::get_bt_uns(), false)
{
   VariableType::BasicType btL = exprL->getType()->getBasicType();

   // Can only be done for BT_ARRAY or BT_STRING.
   if (btL != VariableType::BT_ARR && btL != VariableType::BT_STR)
      ERROR_NP("expected BT_ARRAY or BT_STRING, got: %s", make_string(btL).c_str());
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

   if (exprL->getType()->getBasicType() == VariableType::BT_STR)
   {
      VariableType::Reference type = getType();
      bigsint typeSize = type->getSize(pos);
      bigsint sizeL = exprL->getType()->getSize(pos);
      bigsint sizeR = exprR->getType()->getSize(pos);

      VariableData::Pointer src = VariableData::create_stack(typeSize);

      make_objects_memcpy_prep(objects, dst, src, pos);

      exprL->makeObjects(objects, VariableData::create_stack(sizeL));
      exprR->makeObjects(objects, VariableData::create_stack(sizeR));

      objects->setPosition(pos);

      // 2 = arg count, 15 = native get_char
      objects->addToken(OCODE_NATIVE, objects->getValue(2),
                        objects->getValue(15));

      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }
   else
      ERROR_NP("invalid BT");
}

// EOF

