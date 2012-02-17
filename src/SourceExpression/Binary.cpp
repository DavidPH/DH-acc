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
// Base class for source-level binary expression handling.
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(2)
{
   doCast();
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(bool _arithmetic, SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(_arithmetic)
{
   doCast();
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(VariableType *castL, VariableType *castR,
                        SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(2)
{
   doCast(castL, castR);
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(VariableType *castL, VariableType *castR,
                        bool _arithmetic, SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(_arithmetic)
{
   doCast(castL, castR);
}

//
// SourceExpression_Binary::canMakeObject
//
bool SourceExpression_Binary::canMakeObject() const
{
   return exprL->canMakeObject() && exprR->canMakeObject();
}

//
// SourceExpression_Binary::doCast
//
void SourceExpression_Binary::doCast()
{
   #define PARM context, position

   VariableType::Reference type = getType();

   // Pointer arithmetic.
   if (type->getBasicType() == VariableType::BT_POINTER && arithmetic == 1)
   {
      VariableType::Reference typeRet = type->getReturn();
      bigsint sizeRet = typeRet->getSize(position);

      SourceExpression::Pointer exprOff = create_value_uint(sizeRet, PARM);
      VariableType::Reference typeOff = VariableType::get_bt_uint();

      VariableType::Reference typeL = exprL->getType();
      VariableType::Reference typeR = exprR->getType();

      if (typeL->getBasicType() == VariableType::BT_ARRAY)
      {
         typeL = typeL->getReturn()->getPointer();
         exprL = create_value_cast_implicit(exprL, typeL, PARM);
      }

      if (typeR->getBasicType() == VariableType::BT_ARRAY)
      {
         typeR = typeR->getReturn()->getPointer();
         exprR = create_value_cast_implicit(exprR, typeR, PARM);
      }

      if (typeL->getBasicType() == typeR->getBasicType() &&
          typeL->getUnqualified() != typeR->getUnqualified())
         throw SourceException("VT_POINTER mismatch", position, getName());

      if (typeL->getBasicType() != VariableType::BT_POINTER)
      {
         if (sizeRet != 1)
         {
            exprL = create_value_cast_implicit(exprL, typeOff, PARM);
            exprL = create_binary_mul(exprL, exprOff, PARM);
         }

         exprL = create_value_cast_implicit(exprL, type, PARM);
      }

      if (typeR->getBasicType() != VariableType::BT_POINTER)
      {
         if (sizeRet != 1)
         {
            exprR = create_value_cast_implicit(exprR, typeOff, PARM);
            exprR = create_binary_mul(exprR, exprOff, PARM);
         }

         exprR = create_value_cast_implicit(exprR, type, PARM);
      }

      return;
   }

   exprL = create_value_cast_implicit(exprL, type, PARM);
   exprR = create_value_cast_implicit(exprR, type, PARM);

   #undef PARM
}

//
// SourceExpression_Binary::doCast
//
void SourceExpression_Binary::
doCast(VariableType *castL, VariableType *castR)
{
   if (castL)
      exprL = create_value_cast_implicit(exprL, castL, context, position);

   if (castR)
      exprR = create_value_cast_implicit(exprR, castR, context, position);
}

//
// SourceExpression_Binary::getType
//
VariableType::Reference SourceExpression_Binary::getType() const
{
   return get_promoted_type(exprL->getType(), exprR->getType(), position);
}

//
// SourceExpression_Binary::recurse_makeObjects
//
void SourceExpression_Binary::
recurse_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special case, child handles expressions.
   if (arithmetic == 2) return;

   VariableData::Pointer src =
      VariableData::create_stack(getType()->getSize(position));

   make_objects_memcpy_prep(objects, dst, src, position);
   exprL->makeObjects(objects, src);
   exprR->makeObjects(objects, src);

   objects->setPosition(position);
}

// EOF

