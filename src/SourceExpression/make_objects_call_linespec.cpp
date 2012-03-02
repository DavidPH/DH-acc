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
// SourceExpression handling of calling line specials.
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
// SourceExpression::make_objects_call_linespec
//
void SourceExpression::make_objects_call_linespec
(ObjectVector *objects, VariableData *dst, VariableType *type,
 ObjectExpression *data, Vector const &args, SourceContext *context,
 SourcePosition const &position)
{
   FUNCTION_PREAMBLE
   FUNCTION_ARGS

   ObjectCode ocode;
   ObjectExpression::Pointer ospec = data;

   if (retnSize == 0)
   {
      switch (callSize)
      {
      case 0: objects->addTokenPushZero();
      case 1: ocode = OCODE_ACS_SPECIAL_EXEC1; break;
      case 2: ocode = OCODE_ACS_SPECIAL_EXEC2; break;
      case 3: ocode = OCODE_ACS_SPECIAL_EXEC3; break;
      case 4: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      case 5: ocode = OCODE_ACS_SPECIAL_EXEC5; break;
      default: throw SourceException("bad call-size", position, __func__);
      }
   }
   else if (retnSize == 1)
   {
      switch (callSize)
      {
      case 0: objects->addTokenPushZero();
      case 1: objects->addTokenPushZero();
      case 2: objects->addTokenPushZero();
      case 3: objects->addTokenPushZero();
      case 4: objects->addTokenPushZero();
      case 5: ocode = OCODE_ACSE_SPECIAL_EXEC5_RETN1; break;
      default: throw SourceException("bad call-size", position, __func__);
      }
   }
   else
      throw SourceException("bad return-size", position, __func__);

   objects->addToken(ocode, ospec);

   make_objects_memcpy_post(objects, dst, src, retnType, context, position);
}

// EOF

