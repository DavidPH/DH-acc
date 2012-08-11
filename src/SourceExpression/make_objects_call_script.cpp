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
#include "../SourceContext.hpp"
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
(ObjectVector *objects, VariableData *dst, SourceFunction *func,
 VariableType *type, SourceExpression *data, Vector const &args,
 SourceContext *context, SourcePosition const &pos)
{
   bool named = type->getBasicType() == VariableType::BT_FUN_SNA;

   FUNCTION_PREAMBLE

   data->makeObjects(objects, VariableData::create_stack(type->getSize(pos)));

   FUNCTION_ARGS

   ObjectExpression::Pointer oargc;

   // Determine which OCODE to use.
   ObjectCode ocode;

   int argsSize = callSize;
   if (argsSize > option_script_regargs)
      argsSize = option_script_regargs;

   if(named)
   {
      ocode = OCODE_NATIVE;
      oargc = objects->getValue(argsSize + 1);
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
   if(named)
      ospec = objects->getValue(44); // ACSF_ACS_NamedExecuteWithResult
   else
      ospec = objects->getValue(84); // ACS_ExecuteWithResult

   // Need to handle args not handled by the engine.
   if(callSize > argsSize)
   {
      for(bigsint i = 0, end = callSize - argsSize; i++ != end;)
      {
         objects->addToken(OCODE_GET_IMM, objects->getValue(-i));
         objects->addToken(OCODE_STK_SWAP);
         objects->addToken(OCODE_SET_WLDARR, objects->getValue(option_auto_array));
      }
   }

   // Dummy args.
   if (ocode == OCODE_ACSE_SPECIAL_EXEC5_RETN1)
      for(; argsSize < 5; ++argsSize)
         objects->addTokenPushZero();

   // Save stack pointer.
   objects->addToken(OCODE_GET_AUTPTR);
   objects->addToken(OCODE_SET_TEMP, context->getTempVar(0));

   // The actual call.
   if(named)
   {
      objects->addToken(ocode, oargc, ospec);
      if(retnSize == 0)
         objects->addToken(OCODE_STK_DROP);
   }
   else
      objects->addToken(ocode, ospec);

   // Load stack pointer.
   objects->addToken(OCODE_GET_TEMP, context->getTempVar(0));
   objects->addToken(OCODE_SET_AUTPTR);

   // ZDoom handles one of the return bytes for us.
   if(retnSize >= 1)
      --retnSize;

   // For any return bytes we're handling, push them onto the stack.
   if(retnSize) for(bigsint i = -retnSize; i; ++i)
   {
      objects->addToken(OCODE_GET_IMM, objects->getValue(i));
      objects->addToken(OCODE_GET_WLDARR, objects->getValue(option_auto_array));
   }

   make_objects_memcpy_post(objects, dst, src, retnType, context, pos);
}

// EOF

