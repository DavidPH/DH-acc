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
// SourceExpression handling of calling functions.
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
// SourceExpression::make_objects_call_function
//
void SourceExpression::
make_objects_call_function
(ObjectVector *objects, VariableData *dst, VariableType const *type,
 ObjectExpression *data, std::vector<SourceExpression::Pointer> const &args,
 ObjectExpression *stack, std::string const &labelReturn,
 SourcePosition const &position)
{
   if (args.size() != type->types.size())
      throw SourceException("incorrect arg count to call function", position,
                            __func__);

   VariableData::Pointer src =
      VariableData::create_stack(type->callType->size(position));

   make_objects_memcpy_prep(objects, dst, src, position);

   // Evaluate the arguments.
   for (size_t i = 0; i < args.size(); ++i)
   {
      SourceExpression::Pointer arg = args[i];

      VariableType const *argType = arg->getType();

      if (argType != type->types[i])
         throw SourceException("incorrect arg type to call function",
                               arg->position, __func__);

      VariableData::Pointer argDst =
         VariableData::create_stack(argType->size(position));

      args[i]->makeObjects(objects, argDst);
   }

   objects->setPosition(position);

   // Determine which OCODE to use.
   ObjectCode ocode;
   if (target_type != TARGET_ZDoom)
      ocode = OCODE_BRANCH_GOTO_IMM;
   else if (type->callType->vt == VariableType::VT_VOID)
      ocode = OCODE_ACSE_FUNC_CALLVOID_IMM;
   else
      ocode = OCODE_ACSE_FUNC_CALL_IMM;

   // Determine how many bytes of the return to handle.
   bigsint retnSize = type->callType->size(position);
   // ZDoom handles one of the return bytes for us.
   if (target_type == TARGET_ZDoom && retnSize >= 1)
      --retnSize;

   // Calculate total stack offset.
   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

   // For not ZDoom...
   if (target_type != TARGET_ZDoom)
   {
      // ... Determine how many bytes of the call to handle.
      bigsint callSize = type->sizeCall(position);

      // ... Place args in auto vars.
      // FIXME: Should be based on type.
      for (bigsint i = callSize; i--;)
         objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));

      // ... Push return address.
      ObjectExpression::Pointer retnExpr =
         ObjectExpression::create_value_symbol(labelReturn, position);

      objects->addToken(OCODE_GET_LITERAL32I, retnExpr);
   }

   // The actual call. Data being the jump target.
   objects->addToken(ocode, data);
   objects->addLabel(labelReturn);

   // For any return bytes we're handling, push them onto the stack.
   // FIXME: Should be based on type.
   for (bigsint i(-retnSize); i; ++i)
      objects->addToken(OCODE_GET_AUTO32I, objects->getValue(i));

   // Reset the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_SUB_IMM, ostack);

   make_objects_memcpy_post(objects, dst, src, position);
}

//
// SourceExpression::make_objects_call_function
//
void SourceExpression::
make_objects_call_function
(ObjectVector *objects, VariableData *dst, VariableType const *type,
 SourceExpression *data, std::vector<SourceExpression::Pointer> const &args,
 ObjectExpression *stack, std::string const &labelReturn,
 SourcePosition const &position)
{
   if (args.size() != type->types.size())
      throw SourceException("incorrect arg count to call function", position,
                            __func__);

   VariableData::Pointer src =
      VariableData::create_stack(type->callType->size(position));

   make_objects_memcpy_prep(objects, dst, src, position);

   // Must push return address before target address.
   ObjectExpression::Pointer retnExpr =
      ObjectExpression::create_value_symbol(labelReturn, position);
   objects->addToken(OCODE_GET_LITERAL32I, retnExpr);

   // Determine jump target.
   data->makeObjects(objects, VariableData::create_stack(type->size(position)));

   for (size_t i(0); i < args.size(); ++i)
   {
      SourceExpression::Pointer arg = args[i];

      VariableType const *argType = arg->getType();

      if (argType != type->types[i])
         throw SourceException("incorrect arg type to call function",
                               arg->position, __func__);

      VariableData::Pointer argDst =
         VariableData::create_stack(argType->size(position));

      args[i]->makeObjects(objects, argDst);
   }

   objects->setPosition(position);

   // Determine which OCODE to use.
   ObjectCode ocode = OCODE_BRANCH_GOTO;

   // Determine how many bytes of the return to handle.
   bigsint retnSize(type->callType->size(position));

   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

   // Determine how many bytes of the call to handle.
   bigsint callSize = type->sizeCall(position);

   // Place args in auto vars.
   // FIXME: Should be based on type.
   for (bigsint i(callSize); i--;)
      objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));

   // The actual call.
   objects->addToken(ocode);
   objects->addLabel(labelReturn);

   // For any return bytes we're handling, push them onto the stack.
   // FIXME: Should be based on type.
   for (bigsint i(-retnSize); i; ++i)
      objects->addToken(OCODE_GET_AUTO32I, objects->getValue(i));

   // Reset the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_SUB_IMM, ostack);

   make_objects_memcpy_post(objects, dst, src, position);
}

// EOF

