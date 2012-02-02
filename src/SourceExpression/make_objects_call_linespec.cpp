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
void SourceExpression::
make_objects_call_linespec
(ObjectVector *objects, VariableData *dst, VariableType const *type,
 ObjectExpression *data, std::vector<SourceExpression::Pointer> const &args,
 SourcePosition const &position)
{
   if (args.size() != type->types.size())
      throw SourceException("incorrect arg count to call linespec", position,
                            __func__);

   VariableData::Pointer src =
      VariableData::create_stack(type->callType->size(position));

   make_objects_memcpy_prep(objects, dst, src, position);

   // Evaluate the arguments.
   for (size_t i = 0; i < args.size(); ++i)
   {
      SourceExpression::Pointer arg = args[i];

      VariableType const *argType = arg->getType();

      if (argType != type->types[i])
         throw SourceException("incorrect arg type to call linepsec",
                               arg->position, __func__);

      VariableData::Pointer argDst =
         VariableData::create_stack(argType->size(position));

      args[i]->makeObjects(objects, argDst);
   }

   objects->setPosition(position);

   ObjectCode ocode;
   ObjectExpression::Pointer ospec = data;

   if (type->callType->vt == VariableType::VT_VOID)
   {
      switch (type->sizeCall(position))
      {
      case 0: ocode = OCODE_ACS_SPECIAL_EXEC1; objects->addTokenPushZero(); break;
      case 1: ocode = OCODE_ACS_SPECIAL_EXEC1; break;
      case 2: ocode = OCODE_ACS_SPECIAL_EXEC2; break;
      case 3: ocode = OCODE_ACS_SPECIAL_EXEC3; break;
      case 4: ocode = OCODE_ACS_SPECIAL_EXEC4; break;
      case 5: ocode = OCODE_ACS_SPECIAL_EXEC5; break;
      default: throw SourceException("too many args to call linespec",
                                     position, __func__);
      }
   }
   else
   {
      ocode = OCODE_ACSE_SPECIAL_EXEC5_RETN1;

      for (size_t i(type->sizeCall(position)); i < 5; ++i)
         objects->addTokenPushZero();
   }

   objects->addToken(ocode, ospec);

   make_objects_memcpy_post(objects, dst, src, position);
}

// EOF

