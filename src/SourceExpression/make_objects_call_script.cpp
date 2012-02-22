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

   if (option_named_scripts)
   {
      ocode = OCODE_MISC_NATIVE;
      switch (callSize)
      {
      case  0: oargc = objects->getValue(1); break;
      case  1: oargc = objects->getValue(2); break;
      case  2: oargc = objects->getValue(3); break;
      default: oargc = objects->getValue(4); break;
      }
   }
   else if (retnSize == 0)
   {
      switch (callSize)
      {
      case  0: ocode = OCODE_ACS_SPECIAL_EXEC1; break;
      case  1: ocode = OCODE_ACS_SPECIAL_EXEC2; break;
      case  2: ocode = OCODE_ACS_SPECIAL_EXEC3; break;
      default: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      }
   }
   else if (retnSize == 1)
   {
      switch (callSize)
      {
      case 0: objects->addTokenPushZero();
      case 1: objects->addTokenPushZero();
      case 2: objects->addTokenPushZero();
      default: ocode = OCODE_ACSE_SPECIAL_EXEC5_RETN1; break;
      }
   }
   else
      throw SourceException("bad return-size", position, __func__);

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

   // FIXME: Should be based on type.
   if (callSize > 3) for (bigsint i = callSize - 3; i--;)
      objects->addToken(OCODE_SET_AUTO32I, objects->getValue(i));

   // Dummy arg.
   if (ocode == OCODE_ACSE_SPECIAL_EXEC5_RETN1) objects->addTokenPushZero();

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

