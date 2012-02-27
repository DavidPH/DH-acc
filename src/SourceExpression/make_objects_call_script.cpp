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

extern bool option_script_autoargs;
static option::option_dptr<bool> option_script_autoargs_handlers
('\0', "script-autoargs", "features",
 "Makes script args automatic variables.", NULL, &option_script_autoargs);

extern int option_script_regargs;
static option::option_dptr<int> option_script_regargs_handler
('\0', "script-regargs", "features",
 "Selects how many script arguments get passed using the engine's internal "
 "mechanisms. Defaults to 3.", NULL, &option_script_regargs);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_script_autoargs = false;
int option_script_regargs = 3;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_script
//
void SourceExpression::make_objects_call_script
(ObjectVector *objects, VariableData *dst, VariableType *type,
 SourceExpression *data, Vector const &args, ObjectExpression *stack,
 SourcePosition const &position)
{
   FUNCTION_PREAMBLE

   data->makeObjects(objects, VariableData::create_stack(type->getSize(position)));

   FUNCTION_ARGS

   ObjectExpression::Pointer oargc;

   // Determine which OCODE to use.
   ObjectCode ocode;

   int argsSize = callSize;
   if (argsSize > option_script_regargs)
      argsSize = option_script_regargs;

   if (option_named_scripts)
   {
      ocode = OCODE_MISC_NATIVE;
      switch (argsSize)
      {
      case  0: oargc = objects->getValue(1); break;
      case  1: oargc = objects->getValue(2); break;
      case  2: oargc = objects->getValue(3); break;
      case  3: oargc = objects->getValue(4); break;
      default: oargc = objects->getValue(5); break;
      }
   }
   else if (retnSize == 0)
   {
      switch (argsSize)
      {
      case  0: ocode = OCODE_ACS_SPECIAL_EXEC1; break;
      case  1: ocode = OCODE_ACS_SPECIAL_EXEC2; break;
      case  2: ocode = OCODE_ACS_SPECIAL_EXEC3; break;
      case  3: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      default: ocode = OCODE_ACS_SPECIAL_EXEC5; break;
      }
   }
   else
   {
      ocode = OCODE_ACSE_SPECIAL_EXEC5_RETN1;
   }

   // Determine which line special to use.
   ObjectExpression::Pointer ospec;
   if (option_named_scripts)
      ospec = objects->getValue(44); // ACSF_ACS_NamedExecuteWithResult
   else
      ospec = objects->getValue(84);

   // ZDoom handles one of the return bytes for us.
   if (target_type == TARGET_ZDoom && retnSize >= 1)
      --retnSize;

   // Calculate total stack offset.
   ObjectExpression::Pointer ostack = objects->getValueAdd(stack, retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

   // Need to handle args not handled by the engine.
   if (callSize > option_script_regargs)
   {
      if (option_script_autoargs)
      {
         // FIXME: Should be based on type.
         for (bigsint i = callSize; i-- > option_script_regargs;)
            objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));
      }
      else
      {
         // FIXME: Should be based on type.
         for (bigsint i = callSize - option_script_regargs; i--;)
            objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));
      }
   }

   // Dummy args.
   if (ocode == OCODE_ACSE_SPECIAL_EXEC5_RETN1)
      while (argsSize++ < 4)
         objects->addTokenPushZero();

   // The actual call.
   if (option_named_scripts)
      objects->addToken(ocode, oargc, ospec);
   else
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

