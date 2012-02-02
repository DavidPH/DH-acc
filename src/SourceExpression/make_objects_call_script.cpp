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
// SourceExpression handling of calling scripts.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_script
//
void SourceExpression::
make_objects_call_script
(ObjectVector *objects, VariableData *dst, VariableType const *type,
 SourceExpression *data, std::vector<SourceExpression::Pointer> const &args,
 ObjectExpression *stack, SourcePosition const &position)
{
   if (args.size() != type->types.size())
      throw SourceException("incorrect arg count to call script", position,
                            __func__);

   VariableData::Pointer src =
      VariableData::create_stack(type->callType->size(position));

   make_objects_memcpy_prep(objects, dst, src, position);

   data->makeObjects(objects, VariableData::create_stack(type->size(position)));

   // Evaluate the arguments.
   for (size_t i = 0; i < args.size(); ++i)
   {
      SourceExpression::Pointer arg = args[i];

      VariableType const *argType = arg->getType();

      if (argType != type->types[i])
         throw SourceException("incorrect arg type to call script",
                               arg->position, __func__);

      VariableData::Pointer argDst =
         VariableData::create_stack(argType->size(position));

      args[i]->makeObjects(objects, argDst);
   }

   objects->setPosition(position);

   // Determine which OCODE to use.
   ObjectCode ocode;

   if (type->callType->vt == VariableType::VT_VOID)
   {
      switch (type->sizeCall(position))
      {
      case  0: ocode = OCODE_ACS_SPECIAL_EXEC1; break;
      case  1: ocode = OCODE_ACS_SPECIAL_EXEC2; break;
      case  2: ocode = OCODE_ACS_SPECIAL_EXEC3; break;
      case  3: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      default: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      }
   }
   else
   {
      ocode = OCODE_ACSE_SPECIAL_EXEC5_RETN1;

      // Dummy args.
      for (size_t i(type->sizeCall(position)); i < 3; ++i)
         objects->addTokenPushZero();
   }

   // Determine which line special to use.
   ObjectExpression::Pointer ospec = objects->getValue(84);

   // Determine how many bytes of the return to handle.
   bigsint retnSize = type->callType->size(position);
   // ZDoom handles one of the return bytes for us.
   if (target_type == TARGET_ZDoom && retnSize >= 1)
      --retnSize;

   // Calculate total stack offset.
   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

   // Extended call data.
   bigsint callSize = type->sizeCall(position);

   // FIXME: Should be based on type.
   if (callSize > 3) for (bigsint i = callSize - 3; i--;)
      objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));

   // Dummy arg.
   if (ocode == OCODE_ACSE_SPECIAL_EXEC5_RETN1) objects->addTokenPushZero();

   // The actual call.
   objects->addToken(ocode, ospec);

   // For any return bytes we're handling, push them onto the stack.
   // FIXME: Should be based on type.
   for (bigsint i(-retnSize); i; ++i)
      objects->addToken(OCODE_GET_AUTO32I, objects->getValue(i));

   // Reset the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_SUB_IMM, ostack);

   make_objects_memcpy_post(objects, dst, src, position);
}

// EOF

