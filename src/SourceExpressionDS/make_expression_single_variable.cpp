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
// DS handling of variable declaration.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_extern_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(extern_variable)
{
   SourceContext::NameType const nameType = SourceContext::NT_EXTERN;

   SourceTokenC scToken = in->get(SourceTokenC::TT_IDENTIFIER);
   SourceVariable::StorageClass sc =
      SourceVariable::get_StorageClass(scToken.data, scToken.pos);

   VariableType::Reference type = make_expression_type(in, blocks, context);
   std::string name = in->get(SourceTokenC::TT_IDENTIFIER).data;

   std::string nameObject;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      bigsint address =
         make_expression_single(in, blocks, context)->makeObject()->resolveInt();
      nameObject =
         context->makeNameObject(nameType, sc, type, name, address, token.pos);
   }
   else
   {
      nameObject =
         context->makeNameObject(nameType, sc, type, name, token.pos);
   }

   SourceVariable::Pointer var =
      SourceVariable::create_variable
      (name, type, nameObject, sc, token.pos);

   context->addVariable(var);

   return create_value_variable(var, context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(variable)
{
   bool external = token.data == "__extvar";
   SourceContext::NameType nameType =
      external ? SourceContext::NT_EXTLOCAL : SourceContext::NT_LOCAL;

   SourceTokenC scToken = in->get(SourceTokenC::TT_IDENTIFIER);
   SourceVariable::StorageClass sc =
      SourceVariable::get_StorageClass(scToken.data, scToken.pos);

   VariableType::Reference type = make_expression_type(in, blocks, context);
   std::string name = in->get(SourceTokenC::TT_IDENTIFIER).data;

   std::string nameObject;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      bigsint address =
         make_expression_single(in, blocks, context)->makeObject()->resolveInt();
      nameObject =
         context->makeNameObject(nameType, sc, type, name, address, token.pos);
   }
   else
   {
      nameObject =
         context->makeNameObject(nameType, sc, type, name, token.pos);
   }

   SourceVariable::Pointer var =
      SourceVariable::create_variable(name, type, nameObject, sc, token.pos);

   context->addVariable(var);

   SourceExpression::Pointer expr =
      create_value_variable(var, context, token.pos);

   // Semi-hack so that const vars can be initialized.
   if (in->peekType(SourceTokenC::TT_OP_EQUALS))
   {
      in->get(SourceTokenC::TT_OP_EQUALS);
      expr = create_binary_assign_const
         (expr, make_expression(in, blocks, context), context, token.pos);
   }

   return expr;
}

// EOF

