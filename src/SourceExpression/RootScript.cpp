//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
   SourceExpression_RootScript(VariableType const *type, SRCEXP_EXPR_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   VariableType const *type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_output
//
SourceExpression::Pointer SourceExpression::
create_root_script(VariableType const * type, SRCEXP_EXPR_ARGS)
{
   return new SourceExpression_RootScript(type, context, position);
}

//
// SourceExpression_RootScript::SourceExpression_RootScript
//
SourceExpression_RootScript::
SourceExpression_RootScript(VariableType const *_type, SRCEXP_EXPR_PARM)
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

   bigsint sizeCall = type->sizeCall(position);

   if (sizeCall > 3) for (bigsint i(sizeCall - 3); i--;)
   {
      // FIXME: Should be based on type.
      objects->addToken(OCODE_GET_AUTO_VAR32I, objects->getValue(i));
      objects->addToken(OCODE_SET_REGISTER_VAR32I, objects->getValue(i+3));
   }
}

// EOF

