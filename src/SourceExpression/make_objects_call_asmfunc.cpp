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
// SourceExpression handling of calling assembly functions.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "make_objects_call.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_asmfunc
//
void SourceExpression::make_objects_call_asmfunc
(ObjectVector *objects, VariableData *dst, SourceFunction *func,
 VariableType *type, ObjectExpression *data, Vector const &args,
 SourceContext *context, SourcePosition const &pos)
{
   FUNCTION_PREAMBLE

   ObjectCodeSet ocode = data->resolveOCS();

   bool immediate = ocode.ocode_imm != OCODE_NONE;

   // Do we need to preserve the auto-stack-pointer?
   // TODO: __attribute__((__savestack__))
   bool savestack = ocode_is_delay(ocode.ocode) || ocode_is_delay(ocode.ocode_imm);

   for (size_t i = 0; i < callTypes.size(); ++i)
   {
      if (!callTypes[i])
         Error_P("variadic");

      callSize += callTypes[i]->getSize(pos);

      SourceExpression::Pointer arg;

      if(i < args.size())
         arg = args[i];
      else if(func)
         arg = func->args[i];

      if(!arg)
         Error_P("bad count");

      if (arg->getType() != callTypes[i])
         Error(arg->pos, "bad type");

      immediate = immediate && arg->canMakeObject();
   }

   // Save stack pointer.
   if(savestack)
      make_objects_auto_save(objects, context);

   if (immediate)
   {
      std::vector<ObjectExpression::Pointer> oargs;

      for(size_t i = 0; i < callTypes.size(); ++i)
         oargs.push_back((i < args.size() ? args[i] : func->args[i])->makeObject());

      objects->setPosition(pos);
      objects->addToken(ocode.ocode_imm, oargs);
   }
   else
   {
      if (ocode.ocode == OCODE_NONE)
         Error_P("no ocode");

      for(size_t i = 0; i < callTypes.size(); ++i)
      {
         SourceExpression::Pointer arg = i < args.size() ? args[i] : func->args[i];

         VariableData::Pointer tmp =
            VariableData::create_stack(arg->getType()->getSize(pos));

         arg->makeObjects(objects, tmp);
      }

      objects->setPosition(pos);
      objects->addToken(ocode.ocode);
   }

   // Load stack pointer.
   if(savestack)
      make_objects_auto_load(objects, context);

   make_objects_memcpy_post(objects, dst, src, retnType, context, pos);
}

// EOF

