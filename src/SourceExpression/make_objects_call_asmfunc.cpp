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
(ObjectVector *objects, VariableData *dst, VariableType *type,
 ObjectExpression *data, Vector const &args, SourceContext *context,
 SourcePosition const &position)
{
   FUNCTION_PREAMBLE

   ObjectCodeSet ocode = data->resolveOCode();

   bool immediate = ocode.ocode_imm != OCODE_NONE;

   for (size_t i = 0; i < callTypes.size(); ++i)
   {
      if (!callTypes[i])
         throw SourceException("variadic", position, __func__);

      callSize += callTypes[i]->getSize(position);

      if (i >= args.size())
         throw SourceException("bad count", position, __func__);

      if (args[i]->getType() != callTypes[i])
         throw SourceException("bad type", args[i]->position, __func__);

      immediate = immediate && args[i]->canMakeObject();
   }

   if (immediate)
   {
      std::vector<ObjectExpression::Pointer> oargs;

      for (size_t i = 0; i < args.size(); ++i)
         oargs.push_back(args[i]->makeObject());

      objects->setPosition(position).addToken(ocode.ocode_imm, oargs);
   }
   else
   {
      if (ocode.ocode == OCODE_NONE)
         throw SourceException("no ocode", position, __func__);

      for (size_t i = 0; i < args.size(); ++i)
         args[i]->makeObjects(objects, VariableData::create_stack(args[i]->getType()->getSize(position)));

      objects->setPosition(position).addToken(ocode.ocode);
   }

   make_objects_memcpy_post(objects, dst, src, retnType, context, position);
}

// EOF

