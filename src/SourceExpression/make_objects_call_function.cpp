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

#include "make_objects_call.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../option.hpp"
#include "../ost_type.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern bool option_function_autoargs;
static option::option_dptr<bool> option_function_autoargs_handlers
('\0', "function-autoargs", "features",
 "Makes function args automatic variables.", NULL, &option_function_autoargs);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_function_autoargs = false;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_function
//
void SourceExpression::make_objects_call_function
(ObjectVector *objects, VariableData *dst, VariableType *type,
 ObjectExpression *data, Vector const &args, ObjectExpression *stack,
 std::string const &labelReturn, SourcePosition const &position)
{
   FUNCTION_PREAMBLE
   FUNCTION_ARGS

   // Determine which OCODE to use.
   ObjectCode ocode;
   if (target_type != TARGET_ZDoom)
      ocode = OCODE_BRANCH_GOTO_IMM;
   else if (retnType->getBasicType() == VariableType::BT_VOID)
      ocode = OCODE_ACSE_FUNC_CALLVOID_IMM;
   else
      ocode = OCODE_ACSE_FUNC_CALL_IMM;

   // ZDoom handles one of the return bytes for us.
   if (target_type == TARGET_ZDoom && retnSize >= 1)
      --retnSize;

   // Calculate total stack offset.
   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

   // For not ZDoom or specified auto args...
   if (option_function_autoargs || target_type != TARGET_ZDoom)
   {
      // ... Place args in auto vars.
      // FIXME: Should be based on type.
      for (bigsint i = callSize; i--;)
         objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));
   }

   // For not ZDoom...
   if (target_type != TARGET_ZDoom)
   {
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
void SourceExpression::make_objects_call_function
(ObjectVector *objects, VariableData *dst, VariableType *type,
 SourceExpression *data, Vector const &args,  ObjectExpression *stack,
 std::string const &labelReturn, SourcePosition const &position)
{
   FUNCTION_PREAMBLE

   // Must push return address before target address.
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(labelReturn));

   // Determine jump target.
   data->makeObjects(objects, VariableData::create_stack(type->getSize(position)));

   FUNCTION_ARGS

   // Determine which OCODE to use.
   ObjectCode ocode = OCODE_BRANCH_GOTO;

   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

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

