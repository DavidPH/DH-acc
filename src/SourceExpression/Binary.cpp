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
SourceExpression_Binary(VariableType const *castL, VariableType const *castR,
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
SourceExpression_Binary(VariableType const *castL, VariableType const *castR,
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
   VariableType const * type(getType());

   // Pointer arithmetic.
   if (type->vt == VariableType::VT_POINTER && arithmetic == 1)
   {
      VariableType const * typeL(exprL->getType());
      VariableType const * typeR(exprR->getType());

      if (typeL->vt == VariableType::VT_ARRAY)
      {
         typeL = VariableType::get_pointer(typeL->refType);
         exprL = create_value_cast_implicit(exprL, typeL, context, position);
      }

      if (typeR->vt == VariableType::VT_ARRAY)
      {
         typeR = VariableType::get_pointer(typeR->refType);
         exprR = create_value_cast_implicit(exprR, typeR, context, position);
      }

      if (typeL->vt == typeR->vt && typeL != typeR)
         throw SourceException("VT_POINTER mismatch", position, getName());

      if (typeL->vt != VariableType::VT_POINTER)
      {
         if (type->refType->size(position) != 1)
         {
            if (typeL->vt != VariableType::VT_INT)
               exprL = create_value_cast_implicit
                       (exprL, VariableType::get_vt_int(), context, position);

            exprL = create_binary_mul
                    (exprL, create_value_int
                            (type->refType->size(position), context, position),
                     context, position);
         }

         exprL = create_value_cast_implicit(exprL, type, context, position);
      }

      if (typeR->vt != VariableType::VT_POINTER)
      {
         if (type->refType->size(position) != 1)
         {
            if (typeR->vt != VariableType::VT_INT)
               exprR = create_value_cast_implicit
                       (exprR, VariableType::get_vt_int(), context, position);

            exprR = create_binary_mul
                    (exprR, create_value_int
                            (type->refType->size(position), context, position),
                     context, position);
         }

         exprR = create_value_cast_implicit(exprR, type, context, position);
      }

      return;
   }

   if (exprL->getType() != type)
      exprL = create_value_cast_implicit(exprL, type, context, position);

   if (exprR->getType() != type)
      exprR = create_value_cast_implicit(exprR, type, context, position);
}

//
// SourceExpression_Binary::doCast
//
void SourceExpression_Binary::
doCast(VariableType const *castL, VariableType const *castR)
{
   if (castL && exprL->getType() != castL)
      exprL = create_value_cast_implicit(exprL, castL, context, position);

   if (castR && exprR->getType() != castR)
      exprR = create_value_cast_implicit(exprR, castR, context, position);
}

//
// SourceExpression_Binary::getType
//
VariableType const * SourceExpression_Binary::getType() const
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
      VariableData::create_stack(getType()->size(position));

   make_objects_memcpy_prep(objects, dst, src, position);
   exprL->makeObjects(objects, src);
   exprR->makeObjects(objects, src);

   objects->setPosition(position);
}

// EOF

