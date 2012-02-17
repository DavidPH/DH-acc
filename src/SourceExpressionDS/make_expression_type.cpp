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
// DS handling of types.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::is_expression_type
//
bool SourceExpressionDS::
is_expression_type(std::string const &data, SourceContext *context)
{
   if (data == "__array")    return true;
   if (data == "__asmfunc")  return true;
   if (data == "__block")    return true;
   if (data ==   "enum")     return true;
   if (data == "__function") return true;
   if (data == "__linespec") return true;
   if (data == "__native")   return true;
   if (data == "__script")   return true;
   if (data ==   "struct")   return true;
   if (data == "__typeof")   return true;
   if (data ==   "union")    return true;

   return context->getVariableTypeNull(data);
}

//
// SourceExpressionDS::make_expression_type
//
VariableType::Reference SourceExpressionDS::make_expression_type
(SourceTokenizerDS *in, Vector *blocks, SourceContext *context)
{
   SourceTokenC token = in->get(SourceTokenC::TT_IDENTIFIER);

   bigsint width;
   std::string name;
   VariableType::VecStr names;
   VariableType::Pointer retn;
   VariableType::Pointer type;
   VariableType::Vector types;

   if (token.getData() == "__array")
   {
      retn = make_expression_type(in, blocks, context);
      in->get(SourceTokenC::TT_OP_BRACKET_O);
      width = make_expression(in, blocks, context)->makeObject()->resolveInt();
      in->get(SourceTokenC::TT_OP_BRACKET_C);
      type = retn->getArray(width);
   }
   else if (token.getData() == "__asmfunc")
   {
      make_expression_arglist(in, blocks, context, &types, &retn);

      type = VariableType::get_bt_asmfunc(types, retn);
   }
   else if (token.getData() == "__block")
   {
      in->get(SourceTokenC::TT_OP_BRACE_O);

      while (true)
      {
         if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
         break;

         types.push_back(make_expression_type(in, blocks, context));

         in->get(SourceTokenC::TT_OP_SEMICOLON);
      }

      in->get(SourceTokenC::TT_OP_BRACE_C);

      type = VariableType::get_bt_block(types);
   }
   else if (token.getData() == "enum")
   {
      if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
         name = in->get(SourceTokenC::TT_IDENTIFIER).getData();

      if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_O)
      {
         bigsint enumVal = 0;

         type = context->getVariableType_enum(name, true, token.getPosition());

         in->get(SourceTokenC::TT_OP_BRACE_O);

         if (in->peek().getType() != SourceTokenC::TT_OP_BRACE_C) while (true)
         {
            SourceTokenC enumTok(in->get(SourceTokenC::TT_IDENTIFIER));

            if (in->peek().getType() == SourceTokenC::TT_OP_EQUALS)
            {
               in->get(SourceTokenC::TT_OP_EQUALS);

               enumVal = make_expression
                         (in, blocks, context)->makeObject()->resolveInt();
            }

            ObjectExpression::Pointer enumObj =
               ObjectExpression::create_value_int
               (enumVal++, enumTok.getPosition());

            context->addVariable
            (SourceVariable::create_constant
             (enumTok.getData(), type, enumObj, enumTok.getPosition()));

            if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
               break;

            in->get(SourceTokenC::TT_OP_COMMA);
         }

         in->get(SourceTokenC::TT_OP_BRACE_C);
      }
      else
      {
         type = context->getVariableType_enum(name, false, token.getPosition());
      }
   }
   else if (token.getData() == "__function")
   {
      make_expression_arglist(in, blocks, context, &types, &retn);

      type = VariableType::get_bt_function(types, retn);
   }
   else if (token.getData() == "__linespec")
   {
      make_expression_arglist(in, blocks, context, &types, &retn);

      type = VariableType::get_bt_linespec(types, retn);
   }
   else if (token.getData() == "__native")
   {
      make_expression_arglist(in, blocks, context, &types, &retn);

      type = VariableType::get_bt_native(types, retn);
   }
   else if (token.getData() == "__script")
   {
      make_expression_arglist(in, blocks, context, &types, &retn);

      type = VariableType::get_bt_script(types, retn);
   }
   else if (token.getData() == "struct" || token.getData() == "union")
   {
      bool isUnion = token.getData() == "union";

      type = NULL;

      if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
      {
         name = in->get(SourceTokenC::TT_IDENTIFIER).getData();

         if (isUnion)
            type = context->getVariableType_union(name, token.getPosition());
         else
            type = context->getVariableType_struct(name, token.getPosition());
      }

      if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_O)
      {
         in->get(SourceTokenC::TT_OP_BRACE_O);

         while (true)
         {
            if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
               break;

            types.push_back(make_expression_type(in, blocks, context));
            names.push_back(in->get(SourceTokenC::TT_IDENTIFIER).getData());

            in->get(SourceTokenC::TT_OP_SEMICOLON);
         }

         in->get(SourceTokenC::TT_OP_BRACE_C);

         if (isUnion)
            type = context->getVariableType_union
                   (name, names, types, token.getPosition());
         else
            type = context->getVariableType_struct
                   (name, names, types, token.getPosition());
      }

      if (!type)
      {
         if (isUnion)
            type = context->getVariableType_union(name, token.getPosition());
         else
            type = context->getVariableType_struct(name, token.getPosition());
      }
   }
   else if (token.getData() == "decltype")
   {
      type = make_expression_single(in, blocks, context)->getType();
   }
   else
   {
      type = context->getVariableType(token.getData(), token.getPosition());
   }

   // Suffix modifiers.
   while (true) switch (in->peek().getType())
   {
   case SourceTokenC::TT_IDENTIFIER:
      if (in->peek().getData() == "const")
      {
         SourceTokenC tokenConst = in->get(SourceTokenC::TT_IDENTIFIER);

         if (type->getQualifier(VariableType::QUAL_CONST))
            throw SourceException
                  ("const on const", tokenConst.getPosition(), __func__);

         type = type->setQualifier(VariableType::QUAL_CONST);

         break;
      }
      else
         return type.ref();

   case SourceTokenC::TT_OP_ASTERISK:
      in->get(SourceTokenC::TT_OP_ASTERISK);
      type = type->getPointer();
      break;

   default:
      return type.ref();
   }
}

// EOF

