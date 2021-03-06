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
// SourceExpression handling of calling native functions.
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
// SourceExpression::make_objects_call_native
//
void SourceExpression::make_objects_call_native
(ObjectVector *objects, VariableData *dst, SourceFunction *func,
 VariableType *type, ObjectExpression *data, Vector const &args,
 SourceContext *context, SourcePosition const &pos)
{
   FUNCTION_PREAMBLE
   FUNCTION_ARGS

   objects->addToken(OCODE_NATIVE, objects->getValue(callSize), data);

   // ZDoom always pushes at least one byte.
   if (!retnSize) objects->addToken(OCODE_STK_DROP);

   make_objects_memcpy_post(objects, dst, src, retnType, context, pos);
}

// EOF

