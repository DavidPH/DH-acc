//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// SourceExpression memory movement.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// make_objects_literal
//
// Recursively handles a literal.
//
static void make_objects_literal
(ObjectVector *objects, ObjectExpression *elem, VariableType *type,
 SourcePosition const &pos)
{
   switch (type->getBasicType())
   {
   case VariableType::BT_VOID:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
   case VariableType::BT_UNION:
   case VariableType::BT_ASMFUNC:
      ERROR_P("bad BT: %s", make_string(type->getBasicType()).c_str());

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
   case VariableType::BT_BLOCK:
   case VariableType::BT_STRUCT:
   {
      ObjectExpression::Vector elems;
      elem->expandOnce(&elems);
      VariableType::Vector const &types = type->getTypes();

      if (elems.size() != types.size())
         ERROR_P("incorrect elem count");

      for (size_t i = 0; i < elems.size(); ++i)
         make_objects_literal(objects, elems[i], types[i], pos);
   }
      break;

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_BIT_SFT:
   case VariableType::BT_CHR:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_LABEL:
   case VariableType::BT_PTR:
   case VariableType::BT_PTR_NUL:
   case VariableType::BT_ENUM:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SNUM:
      objects->addToken(OCODE_GET_IMM, elem);
      break;

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
      objects->addToken(OCODE_GET_IMM, elem);
      break;

   case VariableType::BT_FUNCTION:
      objects->addToken(OCODE_GET_FUNCP, elem);
      break;

   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_LL:
      objects->addToken(OCODE_GET_IMM, elem);
      objects->addToken(OCODE_GET_IMM, ObjectExpression::
         create_binary_rsh(elem, objects->getValue(32), pos));
      break;

   case VariableType::BT_STR:
   case VariableType::BT_SNAM:
      objects->addToken(OCODE_GET_STRING, elem);
      break;

   case VariableType::BT_ARR:
   {
      ObjectExpression::Vector elems;
      elem->expandOnce(&elems);
      VariableType::Reference types = type->getReturn();

      if (elems.size() != static_cast<size_t>(type->getWidth()))
         ERROR_P("incorrect elem count");

      for (size_t i = 0; i < elems.size(); ++i)
         make_objects_literal(objects, elems[i], types, pos);
   }
      break;
   }
}

//
// make_objects_memcpy_post_part
//
// Handles a single VariableData's worth of action.
//
static void make_objects_memcpy_post_part
(ObjectVector *objects, VariableData *data, VariableType *type,
 SourceContext *context, bool get, bool set, SourcePosition const pos)
{
   static bigsint const ptrSize = 1;

   static VariableData::Pointer ptrStack = VariableData::create_stack(ptrSize);

   bigsint i;

   if (!data) return;

   switch (data->type)
   {
   //
   // MT_AUTO
   //
   case VariableData::MT_AUTO:
      if (set) for (i = data->size; i--;)
         objects->addToken(OCODE_SET_AUTO,
                           objects->getValueAdd(data->address, i));

      if (get) for (i = 0; i < data->size; ++i)
         objects->addToken(OCODE_GET_AUTO,
                           objects->getValueAdd(data->address, i));

      break;

   //
   // MT_LITERAL
   //
   case VariableData::MT_LITERAL:
      if (set) ERROR_P("MT_LITERAL as dst");
      if (get) make_objects_literal(objects, data->address, type, pos);
      break;

   //
   // MT_NONE
   //
   case VariableData::MT_NONE:
      if (set) ERROR_P("MT_NONE as dst");
      if (get) ERROR_P("MT_NONE as src");
      break;

   //
   // MT_LONGPTR
   //
   case VariableData::MT_LONGPTR:
      if(set) ERROR_P("MT_LONGPTR as dst");
      if(get) ERROR_P("MT_LONGPTR as src");
      break;

   //
   // MT_POINTER
   //
   case VariableData::MT_POINTER:
      if (get && !set)
      {
         // A bit of a hack, but the only time this is already set is from
         // UnaryDecInc. And only for getting.
         if (!data->offsetTemp)
         {
            if (data->offsetExpr)
               data->offsetExpr->makeObjects(objects, ptrStack);
            else
               objects->addTokenPushZero();
         }
      }
      else
      {
         if (!data->offsetTemp)
            data->offsetTemp = ptrStack;

         if (data->offsetTemp->type != VariableData::MT_STACK)
            ERROR_P("offsetTemp not MT_STACK");

         data->offsetExpr->makeObjects(objects, data->offsetTemp);
      }

      // If only get or set and size is 1, only need one copy of the address.
      if (get^set && data->size == 1)
      {
         if (set) objects->addToken(OCODE_SET_PTR, data->address);
         if (get) objects->addToken(OCODE_GET_PTR, data->address);
      }
      else
      {
         ObjectExpression::Pointer tmpA = context->getTempVar(0);
         objects->addToken(OCODE_SET_TEMP, tmpA);

         if (set) for (i = data->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);
            objects->addToken(OCODE_SET_PTR,
                              objects->getValueAdd(data->address, i));
         }

         if (get) for (i = 0; i < data->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);
            objects->addToken(OCODE_GET_PTR,
                              objects->getValueAdd(data->address, i));
         }
      }

      break;

   //
   // MT_REGISTER
   //
   case VariableData::MT_REGISTER:
      if (set) for (i = data->size; i--;)
      {
         switch (data->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_SET_REG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_SET_MAPREG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_SET_WLDREG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_SET_GBLREG,
                              objects->getValueAdd(data->address, i));
            break;
         }
      }

      if (get) for (i = 0; i < data->size; ++i)
      {
         switch (data->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_GET_REG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_GET_MAPREG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_GET_WLDREG,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_GET_GBLREG,
                              objects->getValueAdd(data->address, i));
            break;
         }
      }

      break;

   //
   // MT_ARRAY
   //
   case VariableData::MT_ARRAY:
      if (!data->offsetTemp)
         data->offsetTemp = ptrStack;

      if (data->offsetTemp->type != VariableData::MT_STACK)
         ERROR_P("offsetTemp not MT_STACK");

      // If only get or set and size is 1, only need one copy of the address.
      // Furthermore, that one copy has already been setup by prep.
      if (get^set && data->size == 1)
      {
         if(set) switch(data->sectionA)
         {
         case VariableData::SA_MAP:
            objects->addToken(OCODE_SET_MAPARR, data->address);
            break;

         case VariableData::SA_WORLD:
            objects->addToken(OCODE_SET_WLDARR, data->address);
            break;

         case VariableData::SA_GLOBAL:
            objects->addToken(OCODE_SET_GBLARR, data->address);
            break;
         }

         if (get)
         {
            if (data->offsetExpr)
               data->offsetExpr->makeObjects(objects, data->offsetTemp);
            else
               objects->addTokenPushZero();
         }

         if(get) switch(data->sectionA)
         {
         case VariableData::SA_MAP:
            objects->addToken(OCODE_GET_MAPARR, data->address);
            break;

         case VariableData::SA_WORLD:
            objects->addToken(OCODE_GET_WLDARR, data->address);
            break;

         case VariableData::SA_GLOBAL:
            objects->addToken(OCODE_GET_GBLARR, data->address);
            break;
         }
      }
      else
      {
         if (data->offsetExpr)
            data->offsetExpr->makeObjects(objects, data->offsetTemp);
         else
            objects->addTokenPushZero();

         ObjectExpression::Pointer tmpA = context->getTempVar(0);
         objects->addToken(OCODE_SET_TEMP, tmpA);

         if (set) for (i = data->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);

            if (i)
            {
               objects->addToken(OCODE_GET_IMM, objects->getValue(i));
               objects->addToken(OCODE_ADD_STK_U);
            }

            objects->addToken(OCODE_STK_SWAP);

            switch(data->sectionA)
            {
            case VariableData::SA_MAP:
               objects->addToken(OCODE_SET_MAPARR, data->address);
               break;

            case VariableData::SA_WORLD:
               objects->addToken(OCODE_SET_WLDARR, data->address);
               break;

            case VariableData::SA_GLOBAL:
               objects->addToken(OCODE_SET_GBLARR, data->address);
               break;
            }
         }

         if (get) for (i = 0; i < data->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);

            if (i)
            {
               objects->addToken(OCODE_GET_IMM, objects->getValue(i));
               objects->addToken(OCODE_ADD_STK_U);
            }

            switch(data->sectionA)
            {
            case VariableData::SA_MAP:
               objects->addToken(OCODE_GET_MAPARR, data->address);
               break;

            case VariableData::SA_WORLD:
               objects->addToken(OCODE_GET_WLDARR, data->address);
               break;

            case VariableData::SA_GLOBAL:
               objects->addToken(OCODE_GET_GBLARR, data->address);
               break;
            }
         }
      }
      break;

   //
   // MT_STACK
   //
   case VariableData::MT_STACK:
      // Already on stack, no need to do anything.

      // But if asked to dup, that can only be done for single-byte values.
      if (get && set) ERROR_P("MT_STACK as dup");

      break;

   //
   // MT_STATIC
   //
   case VariableData::MT_STATIC:
      if (set) for (i = data->size; i--;)
         objects->addToken(OCODE_SET_STATIC,
                           objects->getValueAdd(data->address, i));

      if (get) for (i = 0; i < data->size; ++i)
         objects->addToken(OCODE_GET_STATIC,
                           objects->getValueAdd(data->address, i));
      break;

   //
   // MT_STRING
   //
   case VariableData::MT_STRING:
      if(set) ERROR_P("MT_STRING as dst");

      if(get)
      {
         if(data->size == 1)
            objects->addToken(OCODE_NATIVE, objects->getValue(2), objects->getValue(15));
         else
         {
            ObjectExpression::Pointer tmpStr = context->getTempVar(0);
            ObjectExpression::Pointer tmpOff = context->getTempVar(1);

            objects->addToken(OCODE_SET_TEMP, tmpOff);
            objects->addToken(OCODE_SET_TEMP, tmpStr);

            for(i = 0; i < data->size; ++i)
            {
               objects->addToken(OCODE_GET_TEMP, tmpStr);
               objects->addToken(OCODE_GET_TEMP, tmpOff);

               if(i) objects->addToken(OCODE_INC_TEMP_U, tmpOff);

               objects->addToken(OCODE_NATIVE, objects->getValue(2), objects->getValue(15));
            }
         }
      }

      break;

   //
   // MT_VOID
   //
   case VariableData::MT_VOID:
      if (set) for (i = data->size; i--;)
         objects->addToken(OCODE_STK_DROP);

      if (get) ERROR_P("MT_VOID as src");

      break;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_memcpy_prep
//
void SourceExpression::make_objects_memcpy_prep
(ObjectVector *objects, VariableData *dst, VariableData *src,
 SourcePosition const &position)
{
   make_objects_memcpy_prep(objects, NULL, dst, src, position);
}

//
// SourceExpression::make_objects_memcpy_prep
//
void SourceExpression::make_objects_memcpy_prep
(ObjectVector *objects, VariableData *dup, VariableData *dst,
 VariableData *src, SourcePosition const &pos)
{
   static bigsint const ptrSize = 1;

   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
      return;

   objects->setPosition(pos);

   // Set up destination by pushing the address onto the stack now. This way it
   // is there before the data. Of course, this only works with single-byte
   // sources.
   if (!dup && dst->size == 1) switch (dst->type)
   {
   case VariableData::MT_AUTO:
   case VariableData::MT_LONGPTR:
   case VariableData::MT_POINTER:
   case VariableData::MT_REGISTER:
   case VariableData::MT_STACK:
   case VariableData::MT_STATIC:
   case VariableData::MT_STRING:
   case VariableData::MT_VOID:
      break;

   case VariableData::MT_LITERAL:
      ERROR_P("MT_LITERAL as dst");

   case VariableData::MT_NONE:
      ERROR_P("MT_NONE as dst");

   case VariableData::MT_ARRAY:
      if (!dst->offsetTemp)
         dst->offsetTemp = VariableData::create_stack(ptrSize);

      if (dst->offsetExpr)
         dst->offsetExpr->makeObjects(objects, dst->offsetTemp);
      else
         objects->addTokenPushZero();

      if (dst->offsetTemp->type != VariableData::MT_STACK)
         ERROR_P("offsetTemp not MT_STACK");

      break;
   }

   // No need to set up source here, just sanity check.
   if (src) switch (src->type)
   {
   case VariableData::MT_AUTO:
   case VariableData::MT_LITERAL:
   case VariableData::MT_LONGPTR:
   case VariableData::MT_POINTER:
   case VariableData::MT_REGISTER:
   case VariableData::MT_ARRAY:
   case VariableData::MT_STACK:
   case VariableData::MT_STATIC:
   case VariableData::MT_STRING:
      break;

   case VariableData::MT_NONE:
      ERROR_P("MT_NONE as src");

   case VariableData::MT_VOID:
      ERROR_P("MT_VOID as src");
   }
}

//
// SourceExpression::make_objects_memcpy_post
//
void SourceExpression::make_objects_memcpy_post
(ObjectVector *objects, VariableData *dst, VariableData *src,
 VariableType *type, SourceContext *context, SourcePosition const &position)
{
   make_objects_memcpy_post(objects, NULL, dst, src, type, context, position);
}

//
// SourceExpression::make_objects_memcpy_post
//
void SourceExpression::make_objects_memcpy_post
(ObjectVector *objects, VariableData *dup, VariableData *dst,
 VariableData *src, VariableType *type, SourceContext *context,
 SourcePosition const &position)
{
   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
   {
      make_objects_memcpy_void(objects, src, position);
      return;
   }

   // Move src onto the stack.
   make_objects_memcpy_post_part(objects, src, type, context, true, false, position);

   // Move the stack into dst. (And possible back onto the stack.)
   make_objects_memcpy_post_part(objects, dst, type, context, !!dup, true, position);

   // Move the stack into dup.
   make_objects_memcpy_post_part(objects, dup, type, context, false, true, position);
}

//
// SourceExpression::make_objects_memcpy_void
//
void SourceExpression::make_objects_memcpy_void
(ObjectVector *objects, VariableData *src, SourcePosition const &position)
{
   // Only need to do stuff for stuff on the stack.
   if (src->type != VariableData::MT_STACK)
      return;

   objects->setPosition(position);

   for (bigsint i = src->size; i--;)
      objects->addToken(OCODE_STK_DROP);
}

// EOF

