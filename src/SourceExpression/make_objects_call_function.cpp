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
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_function
//
void SourceExpression::make_objects_call_function
(ObjectVector *objects, VariableData *dst, SourceFunction *func,
 VariableType *type, SourceExpression *data, Vector const &args,
 SourceContext *context, SourcePosition const &pos)
{
   unsigned calltype = type->getQualifiers();

   FUNCTION_PREAMBLE
   FUNCTION_ARGS

   // Determine which OCODE to use.
   ObjectCode ocode;
   if(data->canMakeObject())
   {
      if(retnSize == 0)
         ocode = OCODE_JMP_CAL_NIL_IMM;
      else
         ocode = OCODE_JMP_CAL_IMM;
   }
   else
   {
      if(retnSize == 0)
         ocode = OCODE_JMP_CAL_NIL;
      else
         ocode = OCODE_JMP_CAL;
   }

   // ZDoom handles one of the return bytes for us.
   if(retnSize >= 1)
      --retnSize;

   // Calculate total stack offset.
   ObjectExpression::Pointer ostack =
      objects->getValueAdd(context->getLimit(STORE_AUTO), retnSize);

   // Advance the stack-pointer.
   objects->addToken(OCODE_ADD_AUTPTR_IMM, ostack);

   // For autocall...
   if(calltype & VariableType::QUAL_AUTOCALL)
   {
      // ... Place args in auto vars.
      for(bigsint i = callSize; i--;)
         objects->addToken(OCODE_SET_AUTO, objects->getValue(i));
   }

   // The actual call. Data being the jump target.
   if(data->canMakeObject())
      objects->addToken(ocode, data->makeObject());
   else
   {
      data->makeObjects(objects, VariableData::create_stack(1));
      objects->addToken(ocode);
   }

   // For any return bytes we're handling, push them onto the stack.
   if(retnSize) for(bigsint i = -retnSize; i; ++i)
   {
      objects->addToken(OCODE_GET_IMM, objects->getValue(i));
      objects->addToken(OCODE_GET_WLDARR, objects->getValue(option_auto_array));
   }

   // Reset the stack-pointer.
   objects->addToken(OCODE_SUB_AUTPTR_IMM, ostack);

   make_objects_memcpy_post(objects, dst, src, retnType, context, pos);
}

// EOF

