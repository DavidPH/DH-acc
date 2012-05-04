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
#include "../option.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> option_string_tag_handler
('\0', "string-tag", "features",
 "For targets that support it, enables tagging of strings. On by default.",
 "For targets that support it, enables tagging of strings. Turning this off "
 "will very slightly improve performance, but at the cost of ACSE-library "
 "compatibility. On by default.", &option_string_tag);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_named_scripts;
bool option_string_tag = true;


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
   case VariableType::BT_ARRAY:
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

   case VariableType::BT_ASMFUNC:
   case VariableType::BT_FLOAT:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_UNION:
   case VariableType::BT_VOID:
      ERROR_P("bad BT");

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

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_NULLPTR:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SHORT:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      objects->addToken(OCODE_GET_LITERAL32I, elem);
      break;

   case VariableType::BT_FIXED:
   case VariableType::BT_REAL:
      objects->addToken(OCODE_GET_LITERAL32F, elem);
      break;

   case VariableType::BT_LLONG:
   {
      objects->addToken(OCODE_GET_LITERAL32I, elem);

      bigsint value = elem->resolveInt();

      if (0 > value && value > -static_cast<bigsint>(0x100000000))
         value = -1;
      else
         value /= static_cast<bigsint>(0x100000000);

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(value));
   }
      break;

   case VariableType::BT_SCRIPT:
      objects->addToken(OCODE_GET_LITERAL32I, elem);

      if (target_type == TARGET_ZDoom && option_string_tag &&
          option_named_scripts)
         objects->addToken(OCODE_ACSE_STRING_TAG);

      break;

   case VariableType::BT_STRING:
      objects->addToken(OCODE_GET_LITERAL32I, elem);

      if (target_type == TARGET_ZDoom && option_string_tag)
         objects->addToken(OCODE_ACSE_STRING_TAG);

      break;

   case VariableType::BT_ULLONG:
      objects->addToken(OCODE_GET_LITERAL32I, elem);
      objects->addToken(OCODE_GET_LITERAL32I, ObjectExpression::create_binary_div(elem, objects->getValue(0x10000000), pos));
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
         objects->addToken(OCODE_SET_AUTO32I,
                           objects->getValueAdd(data->address, i));

      if (get) for (i = 0; i < data->size; ++i)
         objects->addToken(OCODE_GET_AUTO32I,
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
         if (set)
            objects->addToken(OCODE_SET_POINTER32I, data->address);

         if (get)
            objects->addToken(OCODE_GET_POINTER32I, data->address);
      }
      else
      {
         ObjectExpression::Pointer tmpA = context->getTempVar(0);
         objects->addToken(OCODE_SET_TEMP, tmpA);

         if (set) for (i = data->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);
            objects->addToken(OCODE_SET_POINTER32I,
                              objects->getValueAdd(data->address, i));
         }

         if (get) for (i = 0; i < data->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);
            objects->addToken(OCODE_GET_POINTER32I,
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
            objects->addToken(OCODE_SET_REGISTER32I,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_SET_MAPREGISTER,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_SET_WORLDREGISTER,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALREGISTER,
                              objects->getValueAdd(data->address, i));
            break;
         }
      }

      if (get) for (i = 0; i < data->size; ++i)
      {
         switch (data->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_GET_REGISTER32I,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_GET_MAPREGISTER,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_GET_WORLDREGISTER,
                              objects->getValueAdd(data->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALREGISTER,
                              objects->getValueAdd(data->address, i));
            break;
         }
      }

      break;

   //
   // MT_REGISTERARRAY
   //
   case VariableData::MT_REGISTERARRAY:
      if (!data->offsetTemp)
         data->offsetTemp = ptrStack;

      if (data->offsetTemp->type != VariableData::MT_STACK)
         ERROR_P("offsetTemp not MT_STACK");

      // If only get or set and size is 1, only need one copy of the address.
      // Furthermore, that one copy has already been setup by prep.
      if (get^set && data->size == 1)
      {
         if (set) switch (data->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_SET_MAPARRAY, data->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_SET_WORLDARRAY, data->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALARRAY, data->address);
            break;
         }

         if (get)
         {
            if (data->offsetExpr)
               data->offsetExpr->makeObjects(objects, data->offsetTemp);
            else
               objects->addTokenPushZero();
         }

         if (get) switch (data->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_GET_MAPARRAY, data->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_GET_WORLDARRAY, data->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, data->address);
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
               objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(i));
               objects->addToken(OCODE_ADD32U);
            }

            objects->addToken(OCODE_STACK_SWAP32);

            switch (data->sectionRA)
            {
            case VariableData::SRA_MAP:
               objects->addToken(OCODE_ACSE_SET_MAPARRAY, data->address);
               break;

            case VariableData::SRA_WORLD:
               objects->addToken(OCODE_ACSE_SET_WORLDARRAY, data->address);
               break;

            case VariableData::SRA_GLOBAL:
               objects->addToken(OCODE_ACSE_SET_GLOBALARRAY, data->address);
               break;
            }
         }

         if (get) for (i = 0; i < data->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP, tmpA);

            if (i)
            {
               objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(i));
               objects->addToken(OCODE_ADD32U);
            }

            switch (data->sectionRA)
            {
            case VariableData::SRA_MAP:
               objects->addToken(OCODE_ACSE_GET_MAPARRAY, data->address);
               break;

            case VariableData::SRA_WORLD:
               objects->addToken(OCODE_ACSE_GET_WORLDARRAY, data->address);
               break;

            case VariableData::SRA_GLOBAL:
               objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, data->address);
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
         objects->addToken(OCODE_SET_STATIC32I,
                           objects->getValueAdd(data->address, i));

      if (get) for (i = 0; i < data->size; ++i)
         objects->addToken(OCODE_GET_STATIC32I,
                           objects->getValueAdd(data->address, i));
      break;

   //
   // MT_VOID
   //
   case VariableData::MT_VOID:
      if (set) for (i = data->size; i--;)
         objects->addToken(OCODE_STACK_DROP32);

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
   case VariableData::MT_POINTER:
   case VariableData::MT_REGISTER:
   case VariableData::MT_STACK:
   case VariableData::MT_STATIC:
   case VariableData::MT_VOID:
      break;

   case VariableData::MT_LITERAL:
      ERROR_P("MT_LITERAL as dst");

   case VariableData::MT_NONE:
      ERROR_P("MT_NONE as dst");

   case VariableData::MT_REGISTERARRAY:
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
   case VariableData::MT_POINTER:
   case VariableData::MT_REGISTER:
   case VariableData::MT_REGISTERARRAY:
   case VariableData::MT_STACK:
   case VariableData::MT_STATIC:
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
      objects->addToken(OCODE_STACK_DROP32);
}

// EOF

