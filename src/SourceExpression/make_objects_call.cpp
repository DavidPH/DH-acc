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
// SourceExpression handling of calling.
//
//-----------------------------------------------------------------------------

#include "make_objects_call.hpp"
#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call
//
void SourceExpression::make_objects_call
(ObjectVector *objects, VariableData *dst, SourceExpression *expr,
 Vector const &args, SourceContext *context, SourcePosition const &pos)
{
   SourceFunction::Pointer func;
   VariableType::Reference type = expr->getType();

   if(expr->canGetFunction())
      func = expr->getFunction();

   switch (type->getBasicType())
   {
   case VariableType::BT_FUN:
      make_objects_call_function(objects, dst, func, type, expr, args, context, pos);
      break;

   case VariableType::BT_FUN_ASM:
      if (expr->canMakeObject())
      {
         make_objects_call_asmfunc
         (objects, dst, func, type, expr->makeObject(), args, context, pos);
      }
      else
         Error_P("non-constant asmfunc");
      break;

   case VariableType::BT_FUN_LIN:
      if (expr->canMakeObject())
      {
         make_objects_call_linespec
         (objects, dst, func, type, expr->makeObject(), args, context, pos);
      }
      else
         Error_P("non-constant linespec");
      break;

   case VariableType::BT_FUN_NAT:
      if (expr->canMakeObject())
      {
         make_objects_call_native
         (objects, dst, func, type, expr->makeObject(), args, context, pos);
      }
      else
         Error_P("non-constant native");
      break;

   case VariableType::BT_FUN_SNA:
   case VariableType::BT_FUN_SNU:
      make_objects_call_script(objects, dst, func, type, expr, args, context, pos);
      break;

   default:
      Error_P("attempt to call uncallable");
   }
}

// EOF

