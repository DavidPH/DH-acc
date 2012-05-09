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
// DS handling of argument lists.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ArgList::ArgList
//
SourceExpressionDS::ArgList::ArgList(StoreType _store) : store(_store) {}

//
// SourceExpressionDS::make_expression_arglist
//
void SourceExpressionDS::make_arglist(SourceTokenizerDS *in, Vector *blocks,
   SourceContext *context, ArgList *args)
{
   args->count = 0;

   // Read arguments.
   SourcePosition const pos = in->get(SourceTokenC::TT_OP_PARENTHESIS_O).pos;
   if (!in->peekType(SourceTokenC::TT_OP_PARENTHESIS_C)) while (true)
   {
      args->types.push_back(make_type(in, blocks, context));
      args->count += args->types.back()->getSize(pos);

      if (in->peekType(SourceTokenC::TT_IDENTIFIER))
         args->names.push_back(in->get(SourceTokenC::TT_IDENTIFIER).data);
      else
         args->names.push_back("");

      if (!in->peekType(SourceTokenC::TT_OP_COMMA))
         break;

      in->get(SourceTokenC::TT_OP_COMMA);
   }
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   // (void) should be read as (), unless the void is named.
   if (args->types.size() == 1 && args->names[0].empty() &&
       args->types[0]->getBasicType() == VariableType::BT_VOID)
   {
      args->types.clear();
      args->names.clear();
   }

   // Read return (if it's not already set).
   if (!args->retn)
   {
      if (in->peekType(SourceTokenC::TT_OP_MINUS_GT))
      {
         in->get(SourceTokenC::TT_OP_MINUS_GT);

         args->retn = make_type(in, blocks, context);
      }
      else
         args->retn = VariableType::get_bt_void();
   }

   // Setup context, if provided.
   if (args->context)
   {
      // Set return.
      args->context->setReturnType(args->retn);

      // Add args.
      for (size_t i = 0; i < args->types.size(); ++i)
      {
         VariableType::Pointer argType = args->types[i];
         std::string const &argName = args->names[i];

         std::string argNameObject = args->context->getLabel() + argName;
         SourceVariable::Pointer argVar = SourceVariable::create_variable
            (argName, argType, argNameObject, args->store, pos);

         args->context->addVar(argVar, false, false);
      }
   }
}

// EOF

