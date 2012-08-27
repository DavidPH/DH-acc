//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// C declaration parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::IsDeclaration
//
bool SourceExpressionC::IsDeclaration(SRCEXPC_PARSE_ARG1)
{
   SourceTokenC::Reference tok = in->peek();

   if(tok->type != SourceTokenC::TT_NAM) return false;

   // storage-class-specifier
   if(tok->data == "typedef")  return true;
   if(tok->data == "extern")   return true;
   if(tok->data == "static")   return true;
   if(tok->data == "auto")     return true;
   if(tok->data == "register") return true;

   // type-specifier
   // type-qualifier
   if(IsType(in, context)) return true;

   // function-specifier
   if(tok->data == "inline") return true;

   return false;
}

//
// SourceExpressionC::ParseTypedef
//
SRCEXPC_PARSE_DEFN_EXT(Typedef, DeclarationSpecifiers const &spec)
{
   SourcePosition pos = in->peek()->pos;

   if(spec.fs) Error_P("function-specifier with typedef");

   // A typedef must include at least one declarator.
   if(in->peekType(SourceTokenC::TT_SEMICOLON))
      Error_P("expected declarator");

   // Read a series of declarators as typedefs.
   do
   {
      Declarator decl = ParseDeclarator(spec.type, in, context);

      if(decl.name.empty()) Error_P("expected declarator");

      context->getVariableType_typedef(decl.name, decl.type, pos);
   }
   while(in->dropType(SourceTokenC::TT_COMMA));

   in->get(SourceTokenC::TT_SEMICOLON);

   return create_value_data(context, pos);
}

//
// SourceExpressionC::ParseDeclaration
//
SRCEXPC_PARSE_DEFN_HALF(Declaration)
{
   SourcePosition pos = in->peek()->pos;

   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);

   switch(spec.sc)
   {
   case SC_TYPEDEF: return ParseTypedef(spec, in, context);

   case SC_NONE:
      Error_P("stub");

   case SC_EXTERN:
      Error_P("stub");

   case SC_STATIC:
      Error_P("stub");

   case SC_AUTO:
      Error_P("stub");

   case SC_REGISTER:
      Error_P("stub");
   }

   Error_P("internal error");
}

//
// SourceExpressionC::ParseExternalDeclaration
//
SRCEXPC_PARSE_DEFN_HALF(ExternalDeclaration)
{
   SourcePosition pos = in->peek()->pos;

   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);

   switch(spec.sc)
   {
   case SC_TYPEDEF: return ParseTypedef(spec, in, context);

   case SC_NONE:
      Error_P("stub");

   case SC_EXTERN:
      Error_P("stub");

   case SC_STATIC:
      Error_P("stub");

   case SC_AUTO:
   case SC_REGISTER:
      Error_P("auto or register in external-declaration");
   }

   Error_P("internal error");
}

// EOF

