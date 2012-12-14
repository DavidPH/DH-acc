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
// SourceExpression handling of dummy data.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueData
//
class SourceExpression_ValueData : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueData,
                                   SourceExpression);

public:
   SourceExpression_ValueData
   (bool garbage, VariableType *type, SRCEXP_EXPR_ARGS);

   virtual VariableType::Reference getType() const;

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      return false;
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   VariableType::Reference type;
   bool garbage;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_data
//
SRCEXP_EXPRVAL_DEFN(, data)
{
   return new SourceExpression_ValueData(false,
      VariableType::get_bt_void(), context, pos);
}

//
// SourceExpression::create_value_data
//
SRCEXP_EXPRVAL_DEFN(t, data)
{
   return new SourceExpression_ValueData(false, type, context, pos);
}

//
// SourceExpression::create_value_data_garbage
//
SRCEXP_EXPRVAL_DEFN(t, data_garbage)
{
   return new SourceExpression_ValueData(true, type, context, pos);
}

//
// SourceExpression_ValueData::SourceExpression_ValueData
//
SourceExpression_ValueData::SourceExpression_ValueData
(bool _garbage, VariableType *_type, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS), type(_type), garbage(_garbage)
{
}

//
// SourceExpression_ValueData::getType
//
VariableType::Reference SourceExpression_ValueData::getType() const
{
   return type;
}

//
// SourceExpression_ValueData::virtual_makeObjects
//
void SourceExpression_ValueData::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (dst->type == VariableData::MT_VOID) return;

   bigsint srcSize = type->getSize(pos);

   VariableData::Pointer src = VariableData::create_stack(srcSize);

   make_objects_memcpy_prep(objects, dst, src, pos);

   for (bigsint i = srcSize; i--;)
   {
      // FIXME: Should be based on type.
      if (garbage)
         objects->addToken(OCODE_GET_IMM, objects->getValue(0xDEADBEEF));
      else
         objects->addTokenPushZero();
   }

   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

// EOF

