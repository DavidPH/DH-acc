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
#include "../SourceTokenizerC.hpp"
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
void SourceExpressionDS::make_arglist(SourceTokenizerC *in,
                                      SourceContext *context, ArgList *args)
{
   args->count = 0;

   // Read prefix return. (If one.)
   if(is_type(in, NULL, context))
      args->retn = make_type(in, context);

   // Read name. (If one.)
   {
      std::vector<SourceTokenC::Reference> toks;

      while (in->peekType(SourceTokenC::TT_PAREN_O)) toks.push_back(in->get());

      if(!is_type(in, NULL, context) && in->peekType(SourceTokenC::TT_NAM))
      {
         args->name = in->get()->data;

         for (size_t i = toks.size(); i--;) in->get(SourceTokenC::TT_PAREN_C);
      }
      else
         for (size_t i = toks.size(); i--;) in->unget(toks[i]);
   }

   // Read arguments.
   SourcePosition const pos = in->get(SourceTokenC::TT_PAREN_O)->pos;
   if (!in->peekType(SourceTokenC::TT_PAREN_C)) while (true)
   {
      // ... means variadic, which is marked by a null type.
      if(in->peekType(SourceTokenC::TT_DOT3))
      {
         in->get();
         args->types.push_back(NULL);
         break;
      }

      args->types.push_back(make_type(in, context));
      // If no storage, set to the provided default.
      if(args->types.back()->getStoreType() == STORE_NONE)
         args->types.back() = args->types.back()->setStorage(args->store);
      args->count += args->types.back()->getSize(pos);

      if (in->peekType(SourceTokenC::TT_NAM))
         args->names.push_back(in->get(SourceTokenC::TT_NAM)->data);
      else
         args->names.push_back(std::string());

      if(in->peekType(SourceTokenC::TT_EQUALS))
      {
         in->get();
         SourceExpression::Pointer expr = make_assignment(in, context);
         expr = create_value_cast_implicit(expr, args->types.back(), context, pos);
         args->args.push_back(expr);
      }
      else
         args->args.push_back(NULL);

      if (!in->peekType(SourceTokenC::TT_COMMA)) break;

      in->get(SourceTokenC::TT_COMMA);
   }
   in->get(SourceTokenC::TT_PAREN_C);

   // (void) should be read as (), unless the void is named or defaulted.
   if(args->types.size() == 1 && args->names[0].empty() && !args->args[0] &&
      args->types[0] && args->types[0]->getBasicType() == VariableType::BT_VOID)
   {
      args->types.clear();
      args->names.clear();
      args->args.clear();
   }

   // Read suffix return. (If one.)
   if (!args->retn)
   {
      if (in->peekType(SourceTokenC::TT_MEM))
      {
         in->get(SourceTokenC::TT_MEM);
         args->retn = make_type(in, context);
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
         if(!argType) break;

         std::string const &argName = args->names[i];
         StoreType argStore = argType->getStoreType();

         std::string argNameObject = args->context->getLabel() + argName;
         SourceVariable::Pointer argVar = SourceVariable::create_variable
            (argName, argType, argNameObject, argStore, pos);

         args->context->addVar(argVar, LINKAGE_INTERN, false);
      }
   }
}

// EOF

