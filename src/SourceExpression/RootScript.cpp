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
// SourceExpression handling of the start of a script.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_RootScript
//
class SourceExpression_RootScript : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_RootScript,
                                   SourceExpression);

public:
   SourceExpression_RootScript(VariableType *type, SRCEXP_EXPR_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   VariableType::Reference type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_output
//
SourceExpression::Pointer SourceExpression::
create_root_script(VariableType *type, SRCEXP_EXPR_ARGS)
{
   return new SourceExpression_RootScript(type, context, position);
}

//
// SourceExpression_RootScript::SourceExpression_RootScript
//
SourceExpression_RootScript::
SourceExpression_RootScript(VariableType *_type, SRCEXP_EXPR_PARM)
                            : Super(SRCEXP_EXPR_PASS),
                              type(_type)
{
}

//
// SourceExpression_RootScript::virtual_makeObjects
//
void SourceExpression_RootScript::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint callSize = 0;
   VariableType::Vector const &callTypes = type->getTypes();

   for (size_t i = 0; i < callTypes.size(); ++i)
   {
      if (!callTypes[i])
         throw SourceException("variadic script", position, getName());

      callSize += callTypes[i]->getSize(position);
   }

   if (callSize > 3) for (bigsint i = callSize - 3; i--;)
   {
      // FIXME: Should be based on type.
      objects->addToken(OCODE_GET_AUTO32I, objects->getValue(i));
      objects->addToken(OCODE_SET_REGISTER32I, objects->getValue(i+3));
   }
}

// EOF

