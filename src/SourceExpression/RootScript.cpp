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
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
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
// Global Variables                                                           |
//

extern bool option_script_autoargs;
extern int option_script_regargs;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_output
//
SourceExpression::Pointer SourceExpression::
create_root_script(VariableType *type, SRCEXP_EXPR_ARGS)
{
   return new SourceExpression_RootScript(type, context, pos);
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
         ERROR_NP("variadic script");

      callSize += callTypes[i]->getSize(pos);
   }

   // Acquire new stack pointer.
   if(target_type == TARGET_ZDoom)
      make_objects_auto_alloc(objects, context);

   // Automatic-variable arguments.
   if(option_script_autoargs)
   {
      for(bigsint i = 0; i < callSize && i < option_script_regargs; ++i)
      {
         objects->addToken(OCODE_GET_REG, objects->getValue(i));
         objects->addToken(OCODE_SET_AUTO, objects->getValue(i));
      }
   }

   // Extended arguments.
   if(callSize > option_script_regargs)
   {
      for(bigsint i = -callSize + option_script_regargs; i; ++i)
      {
         objects->addToken(OCODE_GET_IMM, objects->getValue(i));
         objects->addToken(OCODE_GET_WLDARR, objects->getValue(option_auto_array));

         if(option_script_autoargs)
            objects->addToken(OCODE_SET_AUTO, objects->getValue(callSize + i));
         else
            objects->addToken(OCODE_SET_REG, objects->getValue(callSize + i));
      }
   }
}

// EOF

