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
#include "../SourceException.hpp"
#include "../VariableData.hpp"


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// make_objects_memcpy_post_part
//
// Handles a single VariableData's worth of action.
//
static void make_objects_memcpy_post_part
(ObjectVector *objects, VariableData *data, bool get, bool set,
 SourcePosition const position)
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
      if (set)
         throw SourceException("MT_LITERAL as dst", position, __func__);

      if (get)
      {
         objects->addToken(OCODE_GET_LITERAL32I, data->address);

         // ZDoom requires tagging strings.
         if (data->sectionL == VariableData::SL_STRING &&
             target_type == TARGET_ZDoom)
         {
            objects->addToken(OCODE_ACSE_STRING_TAG);
         }
      }

      break;

   //
   // MT_NONE
   //
   case VariableData::MT_NONE:
      if (set)
         throw SourceException("MT_NONE as dst", position, __func__);

      if (get)
         throw SourceException("MT_NONE as src", position, __func__);

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
            throw SourceException("offsetTemp not MT_STACK", position, __func__);

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
         objects->addToken(OCODE_SET_TEMP);

         if (set) for (i = data->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP);
            objects->addToken(OCODE_SET_POINTER32I,
                              objects->getValueAdd(data->address, i));
         }

         if (get) for (i = 0; i < data->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP);
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
         throw SourceException("offsetTemp not MT_STACK", position, __func__);

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

         objects->addToken(OCODE_SET_TEMP);

         if (set) for (i = data->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP);

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
            objects->addToken(OCODE_GET_TEMP);

            if (i)
            {
               objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(i));
               objects->addToken(OCODE_ADD32U);
            }

            objects->addToken(OCODE_STACK_SWAP32);

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
      if (get && set)
         throw SourceException("cannot dup from MT_STACK", position, __func__);

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

      if (get)
         throw SourceException("MT_NONE as src", position, __func__);

      break;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_memcpy_prep
//
void SourceExpression::
make_objects_memcpy_prep(ObjectVector *objects, VariableData *dst,
                         VariableData *src, SourcePosition const &position)
{
   make_objects_memcpy_prep(objects, NULL, dst, src, position);
}

//
// SourceExpression::make_objects_memcpy_prep
//
void SourceExpression::
make_objects_memcpy_prep(ObjectVector *objects, VariableData *dup,
                         VariableData *dst, VariableData *src,
                         SourcePosition const &position)
{
   static bigsint const ptrSize = 1;

   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
      return;

   objects->setPosition(position);

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
      throw SourceException("MT_LITERAL as dst", position, "SourceExpression");

   case VariableData::MT_NONE:
      throw SourceException("MT_NONE as dst", position, "SourceExpression");

   case VariableData::MT_REGISTERARRAY:
      if (!dst->offsetTemp)
         dst->offsetTemp = VariableData::create_stack(ptrSize);

      if (dst->offsetExpr)
         dst->offsetExpr->makeObjects(objects, dst->offsetTemp);
      else
         objects->addTokenPushZero();

      if (dst->offsetTemp->type != VariableData::MT_STACK)
         throw SourceException("offsetTemp not MT_STACK", position,
                               "SourceExpression::make_objects_memcpy_prep");

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
      throw SourceException("MT_NONE as src", position, "SourceExpression");

   case VariableData::MT_VOID:
      throw SourceException("MT_VOID as src", position, "SourceExpression");
   }
}

//
// SourceExpression::make_objects_memcpy_post
//
void SourceExpression::
make_objects_memcpy_post(ObjectVector *objects, VariableData *dst,
                         VariableData *src, SourcePosition const &position)
{
   make_objects_memcpy_post(objects, NULL, dst, src, position);
}

//
// SourceExpression::make_objects_memcpy_post
//
void SourceExpression::
make_objects_memcpy_post(ObjectVector *objects, VariableData *dup,
                         VariableData *dst, VariableData *src,
                         SourcePosition const &position)
{
   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
   {
      make_objects_memcpy_void(objects, src, position);
      return;
   }

   // Move src onto the stack.
   make_objects_memcpy_post_part(objects, src, true, false, position);

   // Move the stack into dst. (And possible back onto the stack.)
   make_objects_memcpy_post_part(objects, dst, !!dup, true, position);

   // Move the stack into dup.
   make_objects_memcpy_post_part(objects, dup, false, true,  position);
}

//
// SourceExpression::make_objects_memcpy_void
//
void SourceExpression::
make_objects_memcpy_void(ObjectVector *objects, VariableData *src,
                         SourcePosition const &position)
{
   // Only need to do stuff for stuff on the stack.
   if (src->type != VariableData::MT_STACK)
      return;

   objects->setPosition(position);

   for (bigsint i = src->size; i--;)
      objects->addToken(OCODE_STACK_DROP32);
}

// EOF

