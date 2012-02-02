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
#include "../SourceException.hpp"
#include "../VariableData.hpp"


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
   static bigsint const ptrSize = 1;

   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
      return;

   objects->setPosition(position);

   // Set up destination by pushing the address onto the stack now. This way it
   // is there before the data. Of course, this only works with single-byte
   // sources.
   if (dst->size == 1) switch (dst->type)
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
   static bigsint const ptrSize = 1;

   static VariableData::Pointer ptrStack = VariableData::create_stack(ptrSize);

   // Special handling for void destination.
   if (dst->type == VariableData::MT_VOID)
   {
      make_objects_memcpy_void(objects, src, position);
      return;
   }

   objects->setPosition(position);

   // Move src onto the stack.
   switch (src->type)
   {
   case VariableData::MT_AUTO:
      for (bigsint i = 0; i < src->size; ++i)
         objects->addToken(OCODE_GET_AUTO32I,
                           objects->getValueAdd(src->address, i));
      break;

   case VariableData::MT_LITERAL:
      objects->addToken(OCODE_GET_LITERAL32I, src->address);
      break;

   case VariableData::MT_NONE:
      throw SourceException("MT_NONE as src", position, "SourceExpression");

   case VariableData::MT_POINTER:
      // A bit of a hack, but the only time this is already set is from
      // UnaryDecInc.
      if (!src->offsetTemp)
      {
         if (src->offsetExpr)
            src->offsetExpr->makeObjects(objects, ptrStack);
         else
            objects->addTokenPushZero();
      }

      if (src->size == 1)
         objects->addToken(OCODE_GET_POINTER32I, src->address);
      else
      {
         objects->addToken(OCODE_SET_TEMP);

         for (bigsint i = 0; i < src->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP);
            objects->addToken(OCODE_GET_POINTER32I,
                              objects->getValueAdd(src->address, i));
         }
      }
      break;

   case VariableData::MT_REGISTER:
      for (bigsint i = 0; i < src->size; ++i)
      {
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_GET_REGISTER32I,
                              objects->getValueAdd(src->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_GET_MAPREGISTER,
                              objects->getValueAdd(src->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_GET_WORLDREGISTER,
                              objects->getValueAdd(src->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALREGISTER,
                              objects->getValueAdd(src->address, i));
            break;
         }
      }
      break;

   case VariableData::MT_REGISTERARRAY:
      if (src->offsetExpr)
         src->offsetExpr->makeObjects(objects, ptrStack);
      else
         objects->addTokenPushZero();

      if (src->size == 1)
      {
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_GET_MAPARRAY, src->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_GET_WORLDARRAY, src->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, src->address);
            break;
         }
      }
      else
      {
         objects->addToken(OCODE_SET_TEMP);

         for (bigsint i = 0; i < src->size; ++i)
         {
            objects->addToken(OCODE_GET_TEMP);

            if (i != 0)
            {
               objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(i));
               objects->addToken(OCODE_ADD32U);
            }

            switch (src->sectionRA)
            {
            case VariableData::SRA_MAP:
               objects->addToken(OCODE_ACSE_GET_MAPARRAY, src->address);
               break;

            case VariableData::SRA_WORLD:
               objects->addToken(OCODE_ACSE_GET_WORLDARRAY, src->address);
               break;

            case VariableData::SRA_GLOBAL:
               objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, src->address);
               break;
            }
         }
      }
      break;

   case VariableData::MT_STACK:
      // Already on stack, no need to do anything.
      break;

   case VariableData::MT_STATIC:
      for (bigsint i = 0; i < src->size; ++i)
         objects->addToken(OCODE_GET_STATIC32I,
                           objects->getValueAdd(src->address, i));
      break;

   case VariableData::MT_VOID:
      throw SourceException("MT_VOID as src", position, "SourceExpression");
   }

   // Move the stack into dst.
   switch (dst->type)
   {
   case VariableData::MT_AUTO:
      for (bigsint i = dst->size; i--;)
         objects->addToken(OCODE_SET_AUTO32I,
                           objects->getValueAdd(dst->address, i));
      break;

   case VariableData::MT_LITERAL:
      throw SourceException("MT_LITERAL as dst", position, "SourceExpression");
      break;

   case VariableData::MT_NONE:
      throw SourceException("MT_NONE as dst", position, "SourceExpression");

   case VariableData::MT_POINTER:
      if (!dst->offsetTemp)
         dst->offsetTemp = VariableData::create_stack(ptrSize);

      if (dst->offsetTemp->type != VariableData::MT_STACK)
      {
         throw SourceException("offsetTemp not MT_STACK", position,
                               "SourceExpression::make_objects_memcpy_prep");
      }

      dst->offsetExpr->makeObjects(objects, dst->offsetTemp);

      if (dst->size == 1)
      {
         objects->addToken(OCODE_SET_POINTER32I, dst->address);
      }
      else
      {
         objects->addToken(OCODE_SET_TEMP);

         for (bigsint i = dst->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP);
            objects->addToken(OCODE_SET_POINTER32I,
                              objects->getValueAdd(dst->address, i));
         }
      }
      break;

   case VariableData::MT_REGISTER:
      for (bigsint i = dst->size; i--;)
      {
         switch (dst->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_SET_REGISTER32I,
                              objects->getValueAdd(dst->address, i));
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_SET_MAPREGISTER,
                              objects->getValueAdd(dst->address, i));
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_SET_WORLDREGISTER,
                              objects->getValueAdd(dst->address, i));
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALREGISTER,
                              objects->getValueAdd(dst->address, i));
            break;
         }
      }
      break;

   case VariableData::MT_REGISTERARRAY:
      if (!dst->offsetTemp)
         dst->offsetTemp = VariableData::create_stack(ptrSize);

      if (dst->offsetTemp->type != VariableData::MT_STACK)
      {
         throw SourceException("offsetTemp not MT_STACK", position,
                               "SourceExpression::make_objects_memcpy_prep");
      }

      if (dst->size == 1)
      {
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_SET_MAPARRAY, src->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_SET_WORLDARRAY, src->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALARRAY, src->address);
            break;
         }
      }
      else
      {
         dst->offsetExpr->makeObjects(objects, dst->offsetTemp);

         objects->addToken(OCODE_SET_TEMP);

         for (bigsint i = dst->size; i--;)
         {
            objects->addToken(OCODE_GET_TEMP);

            if (i)
            {
               objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(i));
               objects->addToken(OCODE_ADD32U);
            }

            objects->addToken(OCODE_STACK_SWAP32);

            switch (src->sectionRA)
            {
            case VariableData::SRA_MAP:
               objects->addToken(OCODE_ACSE_SET_MAPARRAY, src->address);
               break;

            case VariableData::SRA_WORLD:
               objects->addToken(OCODE_ACSE_SET_WORLDARRAY, src->address);
               break;

            case VariableData::SRA_GLOBAL:
               objects->addToken(OCODE_ACSE_SET_GLOBALARRAY, src->address);
               break;
            }
         }
      }
      break;

   case VariableData::MT_STACK:
      // Already on stack, no need to do anything.
      break;

   case VariableData::MT_STATIC:
      for (bigsint i = dst->size; i--;)
         objects->addToken(OCODE_SET_STATIC32I,
                           objects->getValueAdd(dst->address, i));
      break;

   case VariableData::MT_VOID:
      for (bigsint i = dst->size; i--;)
         objects->addToken(OCODE_STACK_DROP32);
      break;
   }
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

