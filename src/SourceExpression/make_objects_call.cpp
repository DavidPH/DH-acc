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
 Vector const &args, ObjectExpression *stack, std::string const &labelReturn,
 SourcePosition const &position)
{
   VariableType::Reference type = expr->getType();

   switch (type->getBasicType())
   {
   case VariableType::BT_ASMFUNC:
      if (expr->canMakeObject())
      {
         make_objects_call_asmfunc(objects, dst, type, expr->makeObject(),
                                   args, position);
      }
      else
      {
         throw SourceException("non-constant asmfunc", position, __func__);
      }
      break;

   case VariableType::BT_FUNCTION:
      if (expr->canMakeObject())
      {
         make_objects_call_function(objects, dst, type, expr->makeObject(),
                                    args, stack, labelReturn, position);
      }
      else
      {
         make_objects_call_function(objects, dst, type, expr, args, stack,
                                    labelReturn, position);
      }
      break;

   case VariableType::BT_LINESPEC:
      if (expr->canMakeObject())
      {
         make_objects_call_linespec(objects, dst, type, expr->makeObject(),
                                    args, position);
      }
      else
      {
         throw SourceException("non-constant linespec", position, __func__);
      }
      break;

   case VariableType::BT_NATIVE:
      if (expr->canMakeObject())
      {
         make_objects_call_native(objects, dst, type, expr->makeObject(), args,
                                  position);
      }
      else
      {
         throw SourceException("non-constant native", position, __func__);
      }
      break;

   case VariableType::BT_SCRIPT:
      make_objects_call_script(objects, dst, type, expr, args, stack,
                               position);
      break;

   default:
      throw SourceException("attempt to call uncallable", position, __func__);
   }
}



// EOF

